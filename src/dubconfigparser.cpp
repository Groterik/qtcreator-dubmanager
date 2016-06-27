#include "dubconfigparser.h"

#include "dubprojectmanagerconstants.h"

#include "dubexception.h"
#include "duboptionspage.h"

#include <QProcess>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>

#include <QDebug>

using namespace DubProjectManager;

namespace {
static const QString defaultConfiguration = " [default]";

void runDubProcess(QProcess& process, const QStringList& args, const QString& directory)
{
    process.setWorkingDirectory(directory);
    process.start(DubOptionsPage::executable(), args);
    if (!process.waitForFinished(DubOptionsPage::timeout() * 1000)) {
        qWarning() << "Process timeout: " << process.program() << args.join(' ');
        throw DubException(QLatin1String("Dub process timeout. Try to change timeout in Options."));
    }
    if (process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0) {
        qWarning() << "Process failed: " << process.program() << args.join(' ');
        throw DubException(QLatin1String("Dub process failed: ") + process.errorString()
                           + "\n" + process.readAllStandardError());
    }
}

QStringList parseList(QByteArray& array)
{
    QStringList result;
    QTextStream stream(array);
    QString line;
    do {
        line = stream.readLine();
        if (line.isNull()) break;
        if (!line.startsWith(QLatin1String(" "))) continue;
        line.remove(0, 2);
        if (line.endsWith(defaultConfiguration)){
            line.chop(defaultConfiguration.length());
            result.push_front(line);
        } else {
            result.push_back(line);
        }
    } while (stream.status() == QTextStream::Ok);
    return result;
}
} // namespace

DubConfigParser::DubConfigParser(const QString &directory, QObject *parent) :
    QObject(parent),
    m_directory(directory)
{
}

const QStringList &DubConfigParser::configurationsList() const
{
    return m_configurations;
}

const QStringList& DubConfigParser::buildTypesList() const
{
    return m_buildTypes;
}

const ConfigurationInfo &DubConfigParser::configurationInfo(const QString &conf) const
{
    StateMap::const_iterator it = m_states.find(conf);
    if (it == m_states.end()) {
        throw DubException(tr("No configuration with name ") + conf);
    }
    return *it;
}

const QString &DubConfigParser::projectName() const
{
    return m_projectName;
}

QStringList DubConfigParser::readList(const QStringList &args)
{
    QProcess dub;
    runDubProcess(dub, args, m_directory);
    QByteArray array(dub.readAllStandardOutput());
    return parseList(array);
}

const QJsonValue &checkPresentation(const QJsonValue& v, QJsonValue::Type type = QJsonValue::String)
{
    if (v.isNull() || v.isUndefined() || v.type() != type) {
        throw DubException(QObject::tr("Bad value in json."));
    }
    return v;
}

const QJsonValue getValue(const QJsonObject &obj, const QStringList& names,
                          QJsonValue::Type type = QJsonValue::String) {
    for (const auto &name : names) {
        QJsonValue result = obj.value(name);
        if (result.isNull() || result.isUndefined()) {
            continue;
        }
        if (result.type() != type) {
            throw DubException(QObject::tr(
                "Value \"%1\" found but type mismathed.").arg(name));
        }
        return result;
    }
    throw DubException(QObject::tr(
        "Values \"%1\" not found.").arg(names.join(", ")));
}

QJsonDocument parseOutput(QByteArray array)
{
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(array, &err);

    if (err.error != QJsonParseError::NoError) {
        throw DubException(err.errorString());
    }

    return doc;
}

QString DubConfigParser::parseDescribe(QByteArray array, const QString &projectName,
                                    const QString &projectDir, ConfigurationInfo &state)
{
    QJsonDocument doc = parseOutput(array);

    QJsonObject root = doc.object();
    QJsonValue nameValue = getValue(root, {QLatin1String("rootPackage"),
                                          QLatin1String("mainPackage")});
    if (!projectName.isEmpty() && projectName != nameValue.toString()) {
        throw DubException("Project name mismatched!");
    }
    QString realProjectName = nameValue.toString();

    QJsonValue packages = getValue(root, {QLatin1String("packages")},
                                   QJsonValue::Array);
    QJsonArray packageArray = packages.toArray();
    QJsonObject packageRoot;
    QMap<QString, QStringList> dependenciesImports;
    QStringList subPackagesFiles;
    foreach (QJsonValue package, packageArray) {
        checkPresentation(package, QJsonValue::Object);

        QJsonObject packageObject = package.toObject();
        QString targetPackageName =
                getValue(packageObject, {QLatin1String("name")}).toString();

        if (targetPackageName == realProjectName) {
            if (!packageRoot.isEmpty()) {
                throw DubException("main package duplicate");
            }
            packageRoot = packageObject;
        } else {
            if (targetPackageName.startsWith(realProjectName + QLatin1Char(':'))) {
                // this is a subPackage
                QProcess dubSp;
                runDubProcess(dubSp, QStringList() << "describe" << targetPackageName, projectDir);
                ConfigurationInfo infoSp;
                parseDescribe(dubSp.readAllStandardOutput(), targetPackageName, projectDir, infoSp);
                subPackagesFiles.append(infoSp.files());
            }
            QString depPath =
                    getValue(packageObject, {QLatin1String("path")}).toString();
            QStringList depImports;
            QJsonArray importPathsArray =
                    getValue(packageObject, {QLatin1String("importPaths")},
                             QJsonValue::Array).toArray();
            foreach (QJsonValue importPathValue, importPathsArray) {
                depImports.append(QDir(depPath).absoluteFilePath(
                                      checkPresentation(importPathValue).toString()));
            }
            dependenciesImports[targetPackageName] = depImports;
        }
    }

    state.m_path = getValue(packageRoot, {"path"}).toString();
    state.m_targetName = getValue(packageRoot, {"targetName"}).toString();
    state.m_workingDirectory =
            getValue(packageRoot, {"workingDirectory"}).toString();
    state.m_targetFilename =
            getValue(packageRoot, {"targetFileName"}).toString();
    state.m_targetType = getValue(packageRoot, {"targetType"}).toString();
    state.m_targetPath = getValue(packageRoot, {"targetPath"}).toString();

    QDir qpath(state.m_path);
    QJsonArray srcArray = getValue(packageRoot, {"files"},
                                   QJsonValue::Array).toArray();
    foreach (QJsonValue src, srcArray) {
        QJsonObject srcObj = checkPresentation(src, QJsonValue::Object).toObject();
        if (getValue(srcObj, {"type", "role"}).toString() == "source") {
            state.m_files.push_back(getValue(srcObj, {"path"}).toString());
            state.m_files.back() = qpath.absoluteFilePath(state.m_files.back());
        }
    }
    state.m_files.append(subPackagesFiles);
    state.m_files.removeDuplicates();

    QJsonArray importPathsArray =
            getValue(packageRoot, {"importPaths"}, QJsonValue::Array).toArray();
    foreach (QJsonValue importPathValue, importPathsArray) {
        state.m_importPaths.push_back(qpath.absoluteFilePath(
            checkPresentation(importPathValue).toString()));
    }
    foreach (const QStringList& di, dependenciesImports) {
        state.m_importPaths += di;
    }
    state.m_importPaths.removeDuplicates();
    return realProjectName;
}

bool DubConfigParser::parse()
{
    try {
        m_errorString.clear();

        m_configurations = readList(QStringList() << "build" << "--annotate" << "--print-configs");
        m_configurations.push_front(Constants::DUB_NO_CONFIG);
        m_buildTypes = readList(QStringList() << "build" << "--annotate" << "--print-builds");
        QProcess dub;
        StateMap map;
        foreach (const QString &conf, m_configurations) {
            QStringList args("describe");
            if (conf != Constants::DUB_NO_CONFIG) {
                args << ("--config=" + conf);
            }
            runDubProcess(dub, args, m_directory);
            m_projectName = parseDescribe(dub.readAllStandardOutput(),m_projectName, m_directory,
                                          map[conf]);
        }
        m_states.swap(map);
    } catch (const QException &ex) {
        m_errorString = ex.what();
        return false;
    } catch (...) {
        m_errorString = tr("Dub parse: Unknown error");
        return false;
    }

    return true;
}

const QString &DubConfigParser::errorString() const
{
    return m_errorString;
}

const QStringList &ConfigurationInfo::files() const
{
    return m_files;
}

const QStringList &ConfigurationInfo::directories() const
{
    return m_directories;
}

const QString &ConfigurationInfo::path() const
{
    return m_path;
}

const QStringList &ConfigurationInfo::importPaths() const
{
    return m_importPaths;
}

const QString &ConfigurationInfo::targetName() const
{
    return m_targetName;
}

const QString &ConfigurationInfo::workingDirectory() const
{
    return m_workingDirectory;
}

const QString &ConfigurationInfo::targetFilename() const
{
    return m_targetFilename;
}

const QString &ConfigurationInfo::targetType() const
{
    return m_targetType;
}

const QString &ConfigurationInfo::targetPath() const
{
    return m_targetPath;
}
