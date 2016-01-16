#ifndef DUBBUILDCONFIGURATION_H
#define DUBBUILDCONFIGURATION_H

#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/namedwidget.h>

QT_FORWARD_DECLARE_CLASS(QCheckBox)
QT_FORWARD_DECLARE_CLASS(QComboBox)
QT_FORWARD_DECLARE_CLASS(QPushButton)

namespace ProjectExplorer {
class BuildInfo;
}

namespace DubProjectManager {

class DubProject;

class DubBuildConfiguration : public ProjectExplorer::BuildConfiguration
{
    Q_OBJECT
public:
    DubBuildConfiguration(ProjectExplorer::Target *target, BuildConfiguration *source);
    DubBuildConfiguration(ProjectExplorer::Target *target);
    DubBuildConfiguration(ProjectExplorer::Target *target, const ProjectExplorer::BuildInfo& info);

    // pure ProjectExplorer::BuildConfiguration

    virtual ProjectExplorer::NamedWidget *createConfigWidget() Q_DECL_OVERRIDE;

    virtual BuildType buildType() const Q_DECL_OVERRIDE;

    // others

    DubProject &dubPoject();
    
signals:
    
public slots:

private:
    DubProject *m_project;
};

class CheckedComboBoxWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CheckedComboBoxWidget(const QString &label, QWidget *parent = 0);
    void setLabel(const QString &label);
    void setCurrentText(const QString &text, const QStringList& list);
    QString currentText() const;
    QString newText() const;
signals:
    void currentTextChanged(QString);
private slots:
    void chooseText();
    void onApply();
    void onCancel();
private:
    void setCurrentText(QString text);

    QComboBox *m_combo;
    QCheckBox *m_check;
    QPushButton *m_apply;
    QPushButton *m_cancel;
    QString m_current;
};

class DubBuildConfigurationWidget : public ProjectExplorer::NamedWidget
{
    Q_OBJECT
public:
    explicit DubBuildConfigurationWidget(DubBuildConfiguration *configuration);
public slots:
    void update();
private:
    DubBuildConfiguration* m_configuration;
    CheckedComboBoxWidget m_ccw;
};

class DubBuildConfigurationFactory : public ProjectExplorer::IBuildConfigurationFactory
{
    Q_OBJECT
public:
    DubBuildConfigurationFactory(QObject *parent = 0);

    // pure ProjectExplorer::IBuildConfigurationFactory

    virtual int priority(const ProjectExplorer::Target *parent) const Q_DECL_OVERRIDE;
    virtual QList<ProjectExplorer::BuildInfo *> availableBuilds(
            const ProjectExplorer::Target *parent) const Q_DECL_OVERRIDE;
    virtual QList<ProjectExplorer::BuildInfo *> availableSetups(
            const ProjectExplorer::Kit *k, const QString &projectPath) const Q_DECL_OVERRIDE;
    virtual int priority(
            const ProjectExplorer::Kit *k, const QString &projectPath) const Q_DECL_OVERRIDE;

    virtual ProjectExplorer::BuildConfiguration *create(
            ProjectExplorer::Target *parent,
            const ProjectExplorer::BuildInfo *info) const Q_DECL_OVERRIDE;

    // used to recreate the runConfigurations when restoring settings
    virtual bool canRestore(const ProjectExplorer::Target *parent,
                            const QVariantMap &map) const Q_DECL_OVERRIDE;
    virtual ProjectExplorer::BuildConfiguration *restore(ProjectExplorer::Target *parent,
                                                         const QVariantMap &map) Q_DECL_OVERRIDE;
    virtual bool canClone(const ProjectExplorer::Target *parent,
                          ProjectExplorer::BuildConfiguration *product) const Q_DECL_OVERRIDE;
    virtual ProjectExplorer::BuildConfiguration *clone(
            ProjectExplorer::Target *parent,
            ProjectExplorer::BuildConfiguration *product) Q_DECL_OVERRIDE;

private:
    bool canHandle(const ProjectExplorer::Target *target) const;
};

} // namespace DubProjectManager

#endif // DUBBUILDCONFIGURATION_H
