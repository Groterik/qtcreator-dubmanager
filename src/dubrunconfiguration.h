#ifndef DUBRUNCONFIGURATION_H
#define DUBRUNCONFIGURATION_H

#include <projectexplorer/target.h>
#include <projectexplorer/runconfiguration.h>

QT_FORWARD_DECLARE_CLASS(QLineEdit)
QT_FORWARD_DECLARE_CLASS(QCheckBox)
QT_FORWARD_DECLARE_CLASS(QComboBox)
QT_FORWARD_DECLARE_CLASS(QLabel)

namespace Utils {
class PathChooser;
class DetailsWidget;
}

namespace DubProjectManager {

class DubProject;

class DubRunConfiguration : public ProjectExplorer::RunConfiguration
{
    Q_OBJECT
public:
    DubRunConfiguration(ProjectExplorer::Target *parent, Core::Id id, DubProject* project);
    DubRunConfiguration(ProjectExplorer::Target *parent, DubRunConfiguration *source);

    // ProjectExplorer::RunConfiguration

    virtual QWidget *createConfigurationWidget() Q_DECL_OVERRIDE;
    virtual ProjectExplorer::Runnable runnable() const Q_DECL_OVERRIDE;

    // others

    using RunMode = ProjectExplorer::ApplicationLauncher::Mode;

    QString executable() const;
    RunMode runMode() const;
    QString workingDirectory() const;
    QString commandLineArguments() const;
    QString configuration() const;
    QStringList configurationsList() const;

    virtual bool fromMap(const QVariantMap &map) Q_DECL_OVERRIDE;
    virtual QVariantMap toMap() const Q_DECL_OVERRIDE;

signals:
    void updated();

public slots:
    void setArguments(const QString &args);
    void setConfiguration(const QString& conf);
    void runInTerminal(bool toggled);

    void update();

private:
    void init();

    DubProject* m_project;
    QString m_configuration;

    RunMode m_runMode;
    QString m_executable;
    QString m_workingDirectory;
    QString m_title;
    QString m_arguments;
    bool m_terminal;

};

class DubRunConfigurationWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DubRunConfigurationWidget(DubRunConfiguration *dubRunConfiguration,
                                       QWidget *parent = 0);

private slots:
    void environmentWasChanged();
    void runConfigurationUpdated();

private:

    bool m_ignoreChange;
    QLineEdit *m_argumentsLineEdit;
    DubRunConfiguration *m_dubRunConfiguration;
    QComboBox *m_configurations;
    QLabel *m_summary;
    QLabel *m_workingDirectory;
    QCheckBox *m_runInTerminal;
    Utils::DetailsWidget *m_detailsContainer;
};

class DubRunConfigurationFactory : public ProjectExplorer::IRunConfigurationFactory
{
    Q_OBJECT

public:
    explicit DubRunConfigurationFactory(QObject *parent = 0);
    ~DubRunConfigurationFactory();

    // pure ProjectExplorer::IRunConfigurationFactory
    bool canCreate(ProjectExplorer::Target *parent, const Core::Id id) const Q_DECL_OVERRIDE;
    bool canRestore(ProjectExplorer::Target *parent, const QVariantMap &map) const Q_DECL_OVERRIDE;
    bool canClone(ProjectExplorer::Target *parent,
                  ProjectExplorer::RunConfiguration *product) const Q_DECL_OVERRIDE;
    ProjectExplorer::RunConfiguration *clone(
            ProjectExplorer::Target *parent, ProjectExplorer::RunConfiguration *product) Q_DECL_OVERRIDE;
    virtual QList<Core::Id> availableCreationIds(ProjectExplorer::Target *parent,
                                                 CreationMode mode) const Q_DECL_OVERRIDE;
    QString displayNameForId(const Core::Id id) const Q_DECL_OVERRIDE;

    static Core::Id idFromBuildTarget(const QString &target);
    static QString buildTargetFromId(Core::Id id);

private:
    bool canHandle(ProjectExplorer::Target *target) const;

    ProjectExplorer::RunConfiguration *doCreate(ProjectExplorer::Target *parent,
                                                const Core::Id id) Q_DECL_OVERRIDE;
    ProjectExplorer::RunConfiguration *doRestore(ProjectExplorer::Target *parent,
                                                 const QVariantMap &map) Q_DECL_OVERRIDE;
};

} // namespace DubProjectManager

#endif // DUBRUNCONFIGURATION_H
