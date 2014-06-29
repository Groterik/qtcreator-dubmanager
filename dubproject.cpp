#include "dubproject.h"

#include "dubprojectmanagerconstants.h"

#include "dubmanager.h"
#include "dubfile.h"
#include "dubprojectnode.h"
#include "dubbuildconfiguration.h"
#include "dubrunconfiguration.h"
#include "dubconfigparser.h"

#include "dubexception.h"

#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/kitmanager.h>
#include <projectexplorer/kit.h>
#include <projectexplorer/target.h>
#include <projectexplorer/buildinfo.h>
#include <coreplugin/icontext.h>

#include <qtsupport/customexecutablerunconfiguration.h>

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
    m_buildDirectory = QFileInfo(m_filename).absoluteDir().absolutePath();

    m_parser = new DubConfigParser(m_buildDirectory);

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

void DubProject::parseConfig()
{
    m_parser->parse();
    const ConfigurationInfo& s = m_parser->configurationInfo(m_parser->configurationsList().front());
    m_files = s.files();
    m_projectName = s.targetName();
    m_rootNode->setDisplayName(m_projectName);
    m_type = s.targetType() == "executable" ? EXECUTABLE : LIBRARY;
}

void DubProject::init()
{
    try {
        update();
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

const QStringList &DubProject::configurationList() const
{
    return m_parser->configurationsList();
}

const QStringList &DubProject::buildTypesList() const
{
    return m_parser->buildTypesList();
}

const QString &DubProject::currentConfiguration() const
{
    return m_configuration;
}

const ConfigurationInfo &DubProject::info(const QString conf)
{
    return m_parser->configurationInfo(conf);
}

void DubProject::update()
{
    try {
        parseConfig();
        buildSourceTree(m_parser->configurationsList().front());
    }
    catch (const DubException& /*ex*/) {
        return;
    }
    catch (...) {
        return;
    }
    emit updated();
}

void DubProject::setCurrentConfiguration(const QString &conf)
{
    if (conf != m_configuration && (conf.isEmpty() || m_parser->configurationsList().contains(conf))) {
        buildSourceTree(conf);
    }
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
                new DubRunConfiguration(t, DubRunConfigurationFactory::idFromBuildTarget(m_projectName), this);
        t->addRunConfiguration(rc);
        t->updateDefaultRunConfigurations();
    }
}

void DubProject::buildSourceTree(const QString& conf)
{
    m_configuration = conf;
    if (conf.isEmpty()) {
        m_files.clear();
        foreach (const QString& c, m_parser->configurationsList()) {
            m_files << m_parser->configurationInfo(c).files();
        }
        m_files.removeDuplicates();
    } else {
        m_files = m_parser->configurationInfo(conf).files();
    }

    // build tree
    m_rootNode->clear();
    m_rootNode->addFiles(m_files, 0);
    m_rootNode->addFilePath(m_filename);
    emit fileListChanged();
}

void DubProject::dubFileChanged(const QString &filename)
{
    Q_UNUSED(filename);
    init();
}
