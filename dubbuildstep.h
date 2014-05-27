#ifndef DUBBUILDSTEP_H
#define DUBBUILDSTEP_H

#include <projectexplorer/abstractprocessstep.h>

class DubBuildStep : public ProjectExplorer::AbstractProcessStep
{
    Q_OBJECT
public:
    explicit DubBuildStep(ProjectExplorer::BuildStepList *bsl, const Core::Id id);

    // pure ProjectExplorer::AbstractProcessStep (BuildStep)

    virtual bool init();

    virtual void run(QFutureInterface<bool> &fi);

    virtual ProjectExplorer::BuildStepConfigWidget *createConfigWidget();

    // others
    
    const QString& additionalArguments() const;

    bool isChecked(ProjectExplorer::Target *t) const;

signals:
    
public slots:
private:
    QString m_additionalArguments;
    QList<ProjectExplorer::Target*> m_checkedTargets;
};

QT_FORWARD_DECLARE_CLASS(QLineEdit)
QT_FORWARD_DECLARE_CLASS(QListWidget)

class DubBuildStepConfigWidget : public ProjectExplorer::BuildStepConfigWidget
{
    Q_OBJECT
public:
    explicit DubBuildStepConfigWidget(DubBuildStep* step);

    // pure ProjectExplorer::BuildStepConfigWidget

    virtual QString summaryText() const;
    virtual QString displayName() const;

    // others

private:
    DubBuildStep *m_step;
    QLineEdit *m_additionalArguments;
    QListWidget *m_buildTargetsList;
};

#endif // DUBBUILDSTEP_H
