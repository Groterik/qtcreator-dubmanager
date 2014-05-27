#include "dubbuildconfiguration.h"
#include "dubproject.h"

#include <projectexplorer/target.h>
#include <projectexplorer/buildinfo.h>
#include <projectexplorer/kit.h>


DubBuildConfiguration::DubBuildConfiguration(ProjectExplorer::Target *target, BuildConfiguration *source) :
    ProjectExplorer::BuildConfiguration(target, source)
{
    Q_UNUSED(target);
    cloneSteps(source);
}

DubBuildConfiguration::DubBuildConfiguration(ProjectExplorer::Target *target, const Core::Id id)
    : ProjectExplorer::BuildConfiguration(target, id)
{
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
    return availableSetups(parent->kit(), parent->project()->projectDirectory());
}

int DubBuildConfigurationFactory::priority(const ProjectExplorer::Kit *k, const QString &projectPath) const
{
    Q_UNUSED(k);
    Q_UNUSED(projectPath);
    return 0;
}

QList<ProjectExplorer::BuildInfo *> DubBuildConfigurationFactory::availableSetups(const ProjectExplorer::Kit *k, const QString &projectPath) const
{
    ProjectExplorer::BuildInfo *info = new ProjectExplorer::BuildInfo(this);
    info->buildDirectory = Utils::FileName::fromString(projectPath);
    info->displayName = "Dub";
    info->kitId = k->id();
    info->supportsShadowBuild = true;
    info->typeName = "DubManager";
    return QList<ProjectExplorer::BuildInfo*>() << info;
}

ProjectExplorer::BuildConfiguration *DubBuildConfigurationFactory::create(ProjectExplorer::Target *parent, const ProjectExplorer::BuildInfo *info) const
{
    return new DubBuildConfiguration(parent, info->kitId);
}

bool DubBuildConfigurationFactory::canRestore(const ProjectExplorer::Target *parent, const QVariantMap &map) const
{
    return false;
}

ProjectExplorer::BuildConfiguration *DubBuildConfigurationFactory::restore(ProjectExplorer::Target *parent, const QVariantMap &map)
{
    return 0;
}

bool DubBuildConfigurationFactory::canClone(const ProjectExplorer::Target *parent, ProjectExplorer::BuildConfiguration *product) const
{
    return false;
}

ProjectExplorer::BuildConfiguration *DubBuildConfigurationFactory::clone(ProjectExplorer::Target *parent, ProjectExplorer::BuildConfiguration *product)
{
    return 0;
}
