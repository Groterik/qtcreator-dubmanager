#include "dubconfigparser.h"

#include "dubexception.h"
#include "duboptionspage.h"

#include <QProcess>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>

using namespace DubProjectManager;

namespace {
static const QString defaultConfiguration = " [default]";

void runDubProcess(QProcess& process, const QStringList& args, const QString& directory)
{
    process.setWorkingDirectory(directory);
    process.start(DubOptionsPage::executable(), args);
    process.waitForFinished(10000);
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
    m_errorString.clear();
    QProcess dub;
    runDubProcess(dub, args, m_directory);

    QStringList result;
    switch (dub.exitStatus()) {
    case QProcess::NormalExit:
        if (dub.exitCode() != 0) {
            m_errorString = dub.readAllStandardError();
        } else {
            QByteArray array(dub.readAllStandardOutput());
            result = parseList(array);
        }
        break;
    case QProcess::CrashExit:
        m_errorString = dub.readAllStandardError();
        break;
    default:
        break;
    }
    return result;
}

const QJsonValue& CheckPresentation(const QJsonValue& v, QJsonValue::Type type = QJsonValue::String)
{
    if (v.isNull() || v.isUndefined() || v.type() != type) {
        throw DubException(QObject::tr("Bad value in json"));
    }
    return v;
}

bool DubConfigParser::parseDescribe(QByteArray array, ConfigurationInfo &state)
{
    try {
        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(array, &err);

        if (err.error != QJsonParseError::NoError) {
            throw DubException(err.errorString());
        }

        QJsonObject root = doc.object();
        QJsonValue nameValue = root.value(QString::fromUtf8("mainPackage"));
        CheckPresentation(nameValue);
        if (!m_projectName.isEmpty() && m_projectName != nameValue.toString()) {
            throw DubException("main package is mutable");
        }
        m_projectName = nameValue.toString();

        QJsonValue packages = root.value(QString::fromUtf8("packages"));
        CheckPresentation(packages, QJsonValue::Array);

        QJsonArray packageArray = packages.toArray();
        QJsonObject packageRoot;
        QMap<QString, QStringList> dependenciesImports;
        foreach (QJsonValue package, packageArray) {
            CheckPresentation(package, QJsonValue::Object);

            QJsonObject packageObject = package.toObject();
            QString targetPackageName = CheckPresentation(packageObject.value("name")).toString();

            if (targetPackageName == m_projectName) {
                if (!packageRoot.isEmpty()) {
                    throw DubException("main package duplicate");
                }
                packageRoot = packageObject;
            } else {
                QString depPath = CheckPresentation(packageObject.value("path")).toString();
                QStringList depImports;
                QJsonArray importPathsArray = CheckPresentation(packageObject.value("importPaths"), QJsonValue::Array).toArray();
                foreach (QJsonValue importPathValue, importPathsArray) {
                    depImports.append(QDir(depPath).absoluteFilePath(CheckPresentation(importPathValue).toString()));
                }
                dependenciesImports[targetPackageName] = depImports;
            }
        }

        state.m_path = CheckPresentation(packageRoot.value("path")).toString();
        state.m_targetName = CheckPresentation(packageRoot.value("targetName")).toString();
        state.m_workingDirectory = CheckPresentation(packageRoot.value("workingDirectory")).toString();
        state.m_targetFilename = CheckPresentation(packageRoot.value("targetFileName")).toString();
        state.m_targetType = CheckPresentation(packageRoot.value("targetType")).toString();
        state.m_targetPath = CheckPresentation(packageRoot.value("targetPath")).toString();

        QDir qpath(state.m_path);
        QJsonArray srcArray = CheckPresentation(packageRoot.value("files"), QJsonValue::Array).toArray();
        foreach (QJsonValue src, srcArray) {
            QJsonObject srcObj = CheckPresentation(src, QJsonValue::Object).toObject();
            if (CheckPresentation(srcObj.value("type")).toString() == "source") {
                state.m_files.push_back(CheckPresentation(srcObj.value("path")).toString());
                state.m_files.back() = qpath.absoluteFilePath(state.m_files.back());
            }
        }
        state.m_files.removeDuplicates();

        QJsonArray importPathsArray = CheckPresentation(packageRoot.value("importPaths"), QJsonValue::Array).toArray();
        foreach (QJsonValue importPathValue, importPathsArray) {
            state.m_importPaths.push_back(qpath.absoluteFilePath(CheckPresentation(importPathValue).toString()));
        }
        foreach (const QStringList& di, dependenciesImports) {
            state.m_importPaths += di;
        }
        state.m_importPaths.removeDuplicates();
    }
    catch (const DubException& ex) {
        m_errorString = ex.description();
        return false;
    }
    catch (...) {
        m_errorString = tr("Unknown error during dub describe parsing");
        return false;
    }
    return true;
}

bool DubConfigParser::parse()
{
    m_errorString.clear();

    m_configurations = readList(QStringList() << "build" << "--annotate" << "--print-configs");
    if (m_configurations.empty()) {
        return false;
    }
    m_buildTypes = readList(QStringList() << "build" << "--annotate" << "--print-builds");
    if (m_buildTypes.empty()) {
        return false;
    }

    QProcess dub;
    StateMap map;
    foreach (const QString &conf, m_configurations) {
        runDubProcess(dub, QStringList() << "describe" << ("--config=" + conf), m_directory);
        switch (dub.exitStatus()) {
        case QProcess::NormalExit:
            if (!parseDescribe(dub.readAllStandardOutput(), map[conf])) {
                return false;
            }
            break;
        case QProcess::CrashExit:
            m_errorString = dub.readAllStandardError();
            return false;
        default:
            break;
        }
    }
    m_states.swap(map);
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
