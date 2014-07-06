#ifndef DUBRUNCONFIGURATION_H
#define DUBRUNCONFIGURATION_H

#include <projectexplorer/target.h>
#include <projectexplorer/localapplicationrunconfiguration.h>

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

class DubRunConfiguration : public ProjectExplorer::LocalApplicationRunConfiguration
{
    Q_OBJECT
public:
    DubRunConfiguration(ProjectExplorer::Target *parent, Core::Id id, DubProject* project);
    DubRunConfiguration(ProjectExplorer::Target *parent, DubRunConfiguration *source);

    // pure ProjectExplorer::LocalApplicationRunConfiguration

    virtual QString executable() const;
    virtual RunMode runMode() const;
    virtual QString workingDirectory() const;
    virtual QString commandLineArguments() const;

    QWidget *createConfigurationWidget();

    // others

    QString configuration() const;
    QStringList configurationsList() const;

    virtual bool fromMap(const QVariantMap &map);
    virtual QVariantMap toMap() const;

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
    explicit DubRunConfigurationWidget(DubRunConfiguration *dubRunConfiguration, QWidget *parent = 0);

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

    bool canCreate(ProjectExplorer::Target *parent, const Core::Id id) const;
    bool canRestore(ProjectExplorer::Target *parent, const QVariantMap &map) const;
    bool canClone(ProjectExplorer::Target *parent, ProjectExplorer::RunConfiguration *product) const;
    ProjectExplorer::RunConfiguration *clone(ProjectExplorer::Target *parent, ProjectExplorer::RunConfiguration *product);

    QList<Core::Id> availableCreationIds(ProjectExplorer::Target *parent) const;
    QString displayNameForId(const Core::Id id) const;

    static Core::Id idFromBuildTarget(const QString &target);
    static QString buildTargetFromId(Core::Id id);

private:
    bool canHandle(ProjectExplorer::Target *target) const;

    ProjectExplorer::RunConfiguration *doCreate(ProjectExplorer::Target *parent, const Core::Id id);
    ProjectExplorer::RunConfiguration *doRestore(ProjectExplorer::Target *parent,
                                                 const QVariantMap &map);
};

} // namespace DubProjectManager

#endif // DUBRUNCONFIGURATION_H
