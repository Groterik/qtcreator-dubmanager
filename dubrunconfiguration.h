#ifndef DUBRUNCONFIGURATION_H
#define DUBRUNCONFIGURATION_H

#include <projectexplorer/target.h>
#include <projectexplorer/localapplicationrunconfiguration.h>

class DubRunConfiguration : public ProjectExplorer::LocalApplicationRunConfiguration
{
    Q_OBJECT
public:
    DubRunConfiguration(ProjectExplorer::Target *parent, Core::Id id, const QString &executable,
                                 const QString &workingDirectory, const QString &title);
    DubRunConfiguration(ProjectExplorer::Target *parent, ProjectExplorer::LocalApplicationRunConfiguration *source);

    // pure ProjectExplorer::LocalApplicationRunConfiguration

    virtual QString executable() const;
    virtual RunMode runMode() const;
    virtual QString workingDirectory() const;
    virtual QString commandLineArguments() const;

    QWidget *createConfigurationWidget();

    // others

signals:
    void updated();

public slots:
    void setArguments(const QString &args);
    void setExecutable(const QString& exec);
    void setWorkingDirectory(const QString& dir);
    void runInTerminal(bool toggled);
    void resetWorkingDirectory();

private:
    void init();

    RunMode m_runMode;
    QString m_executable;
    QString m_workingDirectory;
    QString m_defaultWorkingDirectory;
    QString m_title;
    QString m_arguments;
    bool m_terminal;

};

namespace Utils {
class PathChooser;
class DetailsWidget;
}

QT_FORWARD_DECLARE_CLASS(QLineEdit)
QT_FORWARD_DECLARE_CLASS(QCheckBox)

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
    Utils::PathChooser *m_executableEdit;
    Utils::PathChooser *m_workingDirectoryEdit;
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

#endif // DUBRUNCONFIGURATION_H
