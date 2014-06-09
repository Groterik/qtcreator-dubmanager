#include "dubbuildconfiguration.h"
#include "dubproject.h"
#include "dubbuildstep.h"
#include "dubprojectmanagerconstants.h"

#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/buildsteplist.h>
#include <projectexplorer/target.h>
#include <projectexplorer/buildinfo.h>
#include <projectexplorer/kit.h>
#include <projectexplorer/kitmanager.h>
#include <coreplugin/mimedatabase.h>


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

DubBuildConfiguration::DubBuildConfiguration(ProjectExplorer::Target *target, const ProjectExplorer::BuildInfo &info)
    : ProjectExplorer::BuildConfiguration(target, DubProjectManager::Constants::DUB_BC_ID)
{
    setDisplayName(info.displayName);
    setDefaultDisplayName(info.displayName);
    setBuildDirectory(info.buildDirectory);

    ProjectExplorer::BuildStepList* buildSteps = stepList(ProjectExplorer::Constants::BUILDSTEPS_BUILD);
    
    DubBuildStep *build = new DubBuildStep(buildSteps);

    buildSteps->insertStep(0, build);
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
    return canHandle(parent) ? 0 : -1;
}

QList<ProjectExplorer::BuildInfo *> DubBuildConfigurationFactory::availableBuilds(const ProjectExplorer::Target *parent) const
{
    ProjectExplorer::BuildInfo *info = new ProjectExplorer::BuildInfo(this);
    info->buildDirectory = Utils::FileName::fromString(parent->project()->projectDirectory());
    info->displayName = "new";
    info->kitId = parent->kit()->id();
    info->supportsShadowBuild = true;
    info->typeName = "Dub Manager";
    return QList<ProjectExplorer::BuildInfo*>() << info;
}

int DubBuildConfigurationFactory::priority(const ProjectExplorer::Kit *k, const QString &projectPath) const
{
    return (k && Core::MimeDatabase::findByFile(QFileInfo(projectPath))
            .matchesType(QLatin1String(DubProjectManager::Constants::DUBMIMETYPE))) ? 0 : -1;
}

QList<ProjectExplorer::BuildInfo *> DubBuildConfigurationFactory::availableSetups(const ProjectExplorer::Kit *k, const QString &projectPath) const
{
    Q_UNUSED(k);
    Q_UNUSED(projectPath);
    return QList<ProjectExplorer::BuildInfo*>();
}

ProjectExplorer::BuildConfiguration *DubBuildConfigurationFactory::create(ProjectExplorer::Target *parent, const ProjectExplorer::BuildInfo *info) const
{
    return new DubBuildConfiguration(parent, *info);
}

bool DubBuildConfigurationFactory::canRestore(const ProjectExplorer::Target *parent, const QVariantMap &map) const
{
    Q_UNUSED(parent);
    Q_UNUSED(map);
    return false;
}

ProjectExplorer::BuildConfiguration *DubBuildConfigurationFactory::restore(ProjectExplorer::Target *parent, const QVariantMap &map)
{
    Q_UNUSED(parent);
    Q_UNUSED(map);
    return 0;
}

bool DubBuildConfigurationFactory::canClone(const ProjectExplorer::Target *parent, ProjectExplorer::BuildConfiguration *product) const
{
    if (!parent->project()->supportsKit(parent->kit())) {
        return false;
    }
    return product->id() == DubProjectManager::Constants::DUB_BC_ID && parent->project()->id() == DubProjectManager::Constants::DUBPROJECT_ID;
}

ProjectExplorer::BuildConfiguration *DubBuildConfigurationFactory::clone(ProjectExplorer::Target *parent, ProjectExplorer::BuildConfiguration *product)
{
    if (!canClone(parent, product)) {
        return 0;
    }
    return new DubBuildConfiguration(parent, product);
}

bool DubBuildConfigurationFactory::canHandle(const ProjectExplorer::Target *target) const
{
    if (!target) return false;
    if (!target->project()->supportsKit(target->kit()))
        return false;
    return target->project()->id() == DubProjectManager::Constants::DUBPROJECT_ID;
}
