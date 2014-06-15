#include "dubproject.h"

#include "dubprojectmanagerconstants.h"

#include "dubmanager.h"
#include "dubfile.h"
#include "dubprojectnode.h"
#include "dubbuildconfiguration.h"
#include "dubrunconfiguration.h"

#include "dubexception.h"

#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/kitmanager.h>
#include <projectexplorer/kit.h>
#include <projectexplorer/target.h>
#include <projectexplorer/buildinfo.h>
#include <coreplugin/icontext.h>

#include <qtsupport/customexecutablerunconfiguration.h>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFileSystemWatcher>

DubProject::DubProject(DubManager *manager, const QString &filename)
    : m_manager(manager),
      m_filename(filename),
      m_rootNode(new DubProjectNode(filename))
{
    setId(DubProjectManager::Constants::DUBPROJECT_ID);
    setProjectContext(Core::Context(DubProjectManager::Constants::PROJECTCONTEXT));
    setProjectLanguages(Core::Context(ProjectExplorer::Constants::LANG_CXX));

    m_projectName = QFileInfo(filename).absoluteDir().dirName();

    m_file = new DubFile(filename, this);
    m_watcher = new QFileSystemWatcher(this);
    m_watcher->addPath(filename);

    connect(m_watcher, SIGNAL(fileChanged(QString)), this, SLOT(dubFileChanged(QString)));

    init();

}

DubProject::~DubProject()
{
}

QString DubProject::displayName() const
{
    return m_projectName;
}

Core::IDocument *DubProject::document() const
{
    return m_file;
}

ProjectExplorer::IProjectManager *DubProject::projectManager() const
{
    return m_manager;
}

ProjectExplorer::ProjectNode *DubProject::rootProjectNode() const
{
    return m_rootNode;
}

QStringList DubProject::files(ProjectExplorer::Project::FilesMode fileMode) const
{
    Q_UNUSED(fileMode);
    return m_files;
}

QStringList extractSources(const QJsonObject& root, const QString& filename)
{
    QJsonValue sourcePaths = root.value(QString::fromUtf8("sourcePaths"));
    QStringList directories;
    if (!sourcePaths.isNull() && !sourcePaths.isUndefined()) {
        if (!sourcePaths.isArray()) {
            throw DubException(QObject::tr("Failed to parse sourcePaths in ") + filename);
        } else {
            QJsonArray paths = sourcePaths.toArray();
            foreach (QJsonValue p, paths) {
                if (!p.isString()) {
                    throw DubException(QObject::tr("Failed to parse path in sourcePaths in ") + filename);
                }
                directories.push_back(p.toString());
            }
        }
    }
    return directories;
}

void DubProject::parseConfig()
{
    QString m_buildDirectory;
    m_buildDirectory = QFileInfo(m_filename).absoluteDir().absolutePath();
    QFile file(m_filename);
    if (!file.open(QIODevice::ReadOnly)) return;
    QByteArray blob = file.readAll();
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(blob, &err);

    if (err.error != QJsonParseError::NoError) {
        throw DubException(tr("Failed to parse config: ") + err.errorString());
    }

    QJsonObject root = doc.object();
    QJsonValue name = root.value(QString::fromUtf8("name"));
    if (name.isNull() || name.isUndefined() || !name.isString()) {
        throw DubException(tr("Failed to parse project name in config"));
    }
    QString m_projectName;
    m_projectName = name.toString();

    QStringList m_directories;
    m_directories = extractSources(root, m_filename);

    QJsonValue configurationsValue = root.value(QString::fromUtf8("configurations"));
    QStringList m_configurations;
    m_configurations.clear();
    if (!configurationsValue.isNull() && !configurationsValue.isUndefined()) {
        if (!configurationsValue.isArray()) {
            throw DubException(tr("Failed to parse configurations list in ") + m_filename);
        } else {
            QJsonArray configurationsArray = configurationsValue.toArray();
            foreach (QJsonValue p, configurationsArray) {
                if (!p.isObject()) {
                    throw DubException(tr("Failed to parse configuration object in ") + m_filename);
                }
                else {
                    QJsonObject configuration = p.toObject();
                    QJsonValue confName = configuration.value(QString::fromLatin1("name"));
                    if (confName.isNull() || name.isUndefined() || !name.isString()) {
                        throw DubException(tr("Failed to parse configuration name in ") + m_filename);
                    }
                    m_configurations.push_back(confName.toString());
                    m_directories += extractSources(configuration, m_filename);
                }
            }
        }
    }

    QString targetType = root.value("targetType").toString("none");
    TargetType m_type;
    if (targetType == "executable") {
        m_type = EXECUTABLE;
    } else if (targetType == "library") {
        m_type = LIBRARY;
    } else m_type = NONE;

    this->m_projectName = m_projectName;
    this->m_buildDirectory = m_buildDirectory;
    this->m_directories = m_directories;
    this->m_configurations = m_configurations;
    this->m_type = m_type;
}

void DubProject::init()
{
    try {
        parseConfig();
        buildSourceTree();
        setupTargets();
    }
    catch (const DubException& /*ex*/) {

    }
    catch (...) {

    }
}

const QString &DubProject::buildDirectory() const
{
    return m_buildDirectory;
}

QString DubProject::executable() const
{
    return m_buildDirectory + "/" + m_projectName;
}

QStringList DubProject::scanDirectories(QStringList directories, const QString& root)
{
    QStringList result;
    foreach (const QString& dirname, directories) {
        QString absoluteDirname = QFileInfo(root).absoluteDir().absoluteFilePath(dirname);
        QDirIterator iterator(absoluteDirname, QDirIterator::Subdirectories);
        while (iterator.hasNext()) {
            iterator.next();
            if (!iterator.fileInfo().isDir()) {
                QString filename = iterator.filePath();
                if (filename.endsWith(".d") || filename.endsWith(".di")) {
                    result.push_back(filename);
                }
            }
        }
    }
    return result;
}

void DubProject::setupTargets()
{
    ProjectExplorer::Kit* defaultKit = ProjectExplorer::KitManager::defaultKit();
    ProjectExplorer::IBuildConfigurationFactory* factory = ProjectExplorer::IBuildConfigurationFactory::find(defaultKit, m_filename);
    QList<const ProjectExplorer::BuildInfo *> infos;
    ProjectExplorer::BuildInfo *info = new ProjectExplorer::BuildInfo(factory);
    info->buildDirectory = Utils::FileName::fromString(buildDirectory());
    info->kitId = defaultKit->id();
    info->displayName = QString::fromLatin1("all");
    info->supportsShadowBuild = true;
    info->typeName = "Dub Manager";
    infos.push_back(info);
    this->setup(infos);

    foreach (ProjectExplorer::Target* t, this->targets()) {
        DubRunConfiguration* rc =
                new DubRunConfiguration(t, DubRunConfigurationFactory::idFromBuildTarget(m_projectName), executable(), buildDirectory(), m_projectName);
        t->addRunConfiguration(rc);
        t->updateDefaultRunConfigurations();
    }
}

void DubProject::buildSourceTree()
{
    if (m_directories.empty()) {
        m_directories.push_back(QString::fromLatin1("src"));
        m_directories.push_back(QString::fromLatin1("source"));
    }

    if (m_configurations.empty()) {
        m_configurations.push_back(QString::fromLatin1("app"));
    }

    m_directories.removeDuplicates();
    m_files = scanDirectories(m_directories, m_filename);
    m_files.removeDuplicates();
    m_rootNode->setDisplayName(m_projectName);

    // build tree
    m_rootNode->clear();
    m_rootNode->addFiles(m_files, 0);
    m_rootNode->addFilePath(m_filename);
}

void DubProject::dubFileChanged(const QString &filename)
{
    Q_UNUSED(filename);
    init();
}
