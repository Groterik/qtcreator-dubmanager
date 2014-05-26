#include "dubbuildconfiguration.h"
#include "dubproject.h"


DubBuildConfiguration::DubBuildConfiguration(DubProject *project, ProjectExplorer::Target *target, BuildConfiguration *source) :
    ProjectExplorer::BuildConfiguration(target, source),
    m_project(project)
{
    Q_UNUSED(target);
    cloneSteps(source);
}

DubBuildConfiguration::DubBuildConfiguration(DubProject* project, ProjectExplorer::Target *target, const Core::Id id)
    : ProjectExplorer::BuildConfiguration(target, id),
      m_project(project)
{
}

QString DubBuildConfiguration::buildDirectory() const
{
    return m_project->buildDirectory();
}

ProjectExplorer::NamedWidget *DubBuildConfiguration::createConfigWidget()
{
    return new DubBuildConfigurationWidget(this);
}

ProjectExplorer::BuildConfiguration::BuildType DubBuildConfiguration::buildType() const
{
    return Release;
}


DubBuildConfigurationWidget::DubBuildConfigurationWidget(DubBuildConfiguration* configuration)
    : configuration(configuration)
{
    this->setDisplayName("DubManager");
}


DubBuildConfigurationFactory::DubBuildConfigurationFactory(QObject *parent)
    : ProjectExplorer::IBuildConfigurationFactory(parent)
{
}

int DubBuildConfigurationFactory::priority(const ProjectExplorer::Target *parent) const
{
    Q_UNUSED(parent);
    return 0;
}

QList<ProjectExplorer::BuildInfo *> DubBuildConfigurationFactory::availableBuilds(const ProjectExplorer::Target *parent) const
{
    Q_UNUSED(parent);
    return QList<ProjectExplorer::BuildInfo*>();
}

int DubBuildConfigurationFactory::priority(const ProjectExplorer::Kit *k, const QString &projectPath) const
{
    Q_UNUSED(k);
    Q_UNUSED(projectPath);
    return 0;
}

QList<ProjectExplorer::BuildInfo *> DubBuildConfigurationFactory::availableSetups(const ProjectExplorer::Kit *k, const QString &projectPath) const
{

}

ProjectExplorer::BuildConfiguration *DubBuildConfigurationFactory::create(ProjectExplorer::Target *parent, const ProjectExplorer::BuildInfo *info) const
{

}

bool DubBuildConfigurationFactory::canRestore(const ProjectExplorer::Target *parent, const QVariantMap &map) const
{

}

ProjectExplorer::BuildConfiguration *DubBuildConfigurationFactory::restore(ProjectExplorer::Target *parent, const QVariantMap &map)
{

}

bool DubBuildConfigurationFactory::canClone(const ProjectExplorer::Target *parent, ProjectExplorer::BuildConfiguration *product) const
{

}

ProjectExplorer::BuildConfiguration *DubBuildConfigurationFactory::clone(ProjectExplorer::Target *parent, ProjectExplorer::BuildConfiguration *product)
{

}
