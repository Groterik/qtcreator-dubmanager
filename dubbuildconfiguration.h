#ifndef DUBBUILDCONFIGURATION_H
#define DUBBUILDCONFIGURATION_H

#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/namedwidget.h>

class DubProject;

class DubBuildConfiguration : public ProjectExplorer::BuildConfiguration
{
    Q_OBJECT
public:
    DubBuildConfiguration(ProjectExplorer::Target *target, BuildConfiguration *source);
    DubBuildConfiguration(ProjectExplorer::Target *target, const Core::Id id);
    DubBuildConfiguration(ProjectExplorer::Target *target, const ProjectExplorer::BuildInfo& info);

    // pure ProjectExplorer::BuildConfiguration

    virtual ProjectExplorer::NamedWidget *createConfigWidget();

    virtual BuildType buildType() const;

    // others

    DubProject &dubPoject();
    
signals:
    
public slots:

private:
    DubProject *m_project;
};

QT_FORWARD_DECLARE_CLASS(QCheckBox)
QT_FORWARD_DECLARE_CLASS(QComboBox)

class DubBuildConfigurationWidget : public ProjectExplorer::NamedWidget
{
    Q_OBJECT
public:
    explicit DubBuildConfigurationWidget(DubBuildConfiguration *configuration);
signals:
    void configurationChoosed(QString conf);
public slots:
    void update();
private slots:
    void chooseConfiguration();
private:
    DubBuildConfiguration* m_configuration;
    QCheckBox *m_chooseConfigs;
    QComboBox *m_configs;
};

namespace ProjectExplorer {
class BuildInfo;
}

class DubBuildConfigurationFactory : public ProjectExplorer::IBuildConfigurationFactory
{
    Q_OBJECT
public:
    DubBuildConfigurationFactory(QObject *parent = 0);

    // pure ProjectExplorer::IBuildConfigurationFactory

    virtual int priority(const ProjectExplorer::Target *parent) const;
    virtual QList<ProjectExplorer::BuildInfo *> availableBuilds(const ProjectExplorer::Target *parent) const;

    virtual int priority(const ProjectExplorer::Kit *k, const QString &projectPath) const;
    virtual QList<ProjectExplorer::BuildInfo *> availableSetups(const ProjectExplorer::Kit *k, const QString &projectPath) const;

    virtual ProjectExplorer::BuildConfiguration *create(ProjectExplorer::Target *parent, const ProjectExplorer::BuildInfo *info) const;

    // used to recreate the runConfigurations when restoring settings
    virtual bool canRestore(const ProjectExplorer::Target *parent, const QVariantMap &map) const;
    virtual ProjectExplorer::BuildConfiguration *restore(ProjectExplorer::Target *parent, const QVariantMap &map);
    virtual bool canClone(const ProjectExplorer::Target *parent, ProjectExplorer::BuildConfiguration *product) const;
    virtual ProjectExplorer::BuildConfiguration *clone(ProjectExplorer::Target *parent, ProjectExplorer::BuildConfiguration *product);

    // others

    bool canHandle(const ProjectExplorer::Target *target) const;

private:
};

#endif // DUBBUILDCONFIGURATION_H
