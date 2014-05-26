#ifndef DUBBUILDCONFIGURATION_H
#define DUBBUILDCONFIGURATION_H

#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/namedwidget.h>

class DubProject;

class DubBuildConfiguration : public ProjectExplorer::BuildConfiguration
{
    Q_OBJECT
public:
    DubBuildConfiguration(DubProject* project, ProjectExplorer::Target *target, BuildConfiguration *source);
    DubBuildConfiguration(DubProject* project, ProjectExplorer::Target *target, const Core::Id id);

    // pure ProjectExplorer::BuildConfiguration

    virtual QString buildDirectory() const;

    virtual ProjectExplorer::NamedWidget *createConfigWidget();

    virtual BuildType buildType() const;

    // others
    
signals:
    
public slots:

private:
    DubProject* m_project;
};

class DubBuildConfigurationWidget : public ProjectExplorer::NamedWidget
{
    Q_OBJECT
public:
    explicit DubBuildConfigurationWidget(DubBuildConfiguration *configuration);
private:
    DubBuildConfiguration* configuration;
};

namespace ProjectExplorer {
class BuildInfo;
}

class DubBuildConfigurationFactory : public ProjectExplorer::IBuildConfigurationFactory
{
    Q_OBJECT
public:
    DubBuildConfigurationFactory(QObject *parent);

    // pure ProjectExplorer::IBuildConfigurationFactory

    virtual int priority(const ProjectExplorer::Target *parent) const;
    virtual QList<ProjectExplorer::BuildInfo *> availableBuilds(const ProjectExplorer::Target *parent) const;

    virtual int priority(const ProjectExplorer::Kit *k, const QString &projectPath) const;
    // List of build information that can be used to initially set up a new build configuration.
    virtual QList<ProjectExplorer::BuildInfo *> availableSetups(const ProjectExplorer::Kit *k, const QString &projectPath) const;

    virtual ProjectExplorer::BuildConfiguration *create(ProjectExplorer::Target *parent, const ProjectExplorer::BuildInfo *info) const;

    // used to recreate the runConfigurations when restoring settings
    virtual bool canRestore(const ProjectExplorer::Target *parent, const QVariantMap &map) const;
    virtual ProjectExplorer::BuildConfiguration *restore(ProjectExplorer::Target *parent, const QVariantMap &map);
    virtual bool canClone(const ProjectExplorer::Target *parent, ProjectExplorer::BuildConfiguration *product) const;
    virtual ProjectExplorer::BuildConfiguration *clone(ProjectExplorer::Target *parent, ProjectExplorer::BuildConfiguration *product);

private:
};

#endif // DUBBUILDCONFIGURATION_H
