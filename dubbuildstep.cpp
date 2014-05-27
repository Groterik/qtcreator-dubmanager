#include "dubbuildstep.h"

#include "dubproject.h"

#include <projectexplorer/target.h>

#include <QFormLayout>
#include <QLineEdit>
#include <QListWidget>

DubBuildStep::DubBuildStep(ProjectExplorer::BuildStepList *bsl, const Core::Id id) :
    ProjectExplorer::AbstractProcessStep(bsl, id)
{
}

bool DubBuildStep::init()
{
    return true;
}

void DubBuildStep::run(QFutureInterface<bool> &fi)
{

}

ProjectExplorer::BuildStepConfigWidget *DubBuildStep::createConfigWidget()
{
    return new DubBuildStepConfigWidget(this);
}

const QString &DubBuildStep::additionalArguments() const
{
    return m_additionalArguments;
}

bool DubBuildStep::isChecked(ProjectExplorer::Target *t) const
{
    return m_checkedTargets.contains(t);
}


DubBuildStepConfigWidget::DubBuildStepConfigWidget(DubBuildStep *step)
    : ProjectExplorer::BuildStepConfigWidget(), m_step(step)
{
    QFormLayout *fl = new QFormLayout(this);
    fl->setMargin(0);
    fl->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    setLayout(fl);

    m_additionalArguments = new QLineEdit(this);
    fl->addRow(tr("Additional arguments:"), m_additionalArguments);
    m_additionalArguments->setText(m_step->additionalArguments());

    m_buildTargetsList = new QListWidget;
    m_buildTargetsList->setMinimumHeight(200);
    fl->addRow(tr("Targets:"), m_buildTargetsList);

    ProjectExplorer::Project *pro = m_step->project();
    QList<ProjectExplorer::Target*> targetList = pro->targets();
    foreach (ProjectExplorer::Target* target, targetList) {
        QListWidgetItem *item = new QListWidgetItem(target->displayName(), m_buildTargetsList);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(m_step->isChecked(target) ? Qt::Checked : Qt::Unchecked);
    }
}

QString DubBuildStepConfigWidget::summaryText() const
{
    return QString::fromLatin1("WowSummarytext");
}

QString DubBuildStepConfigWidget::displayName() const
{
    return QString::fromLatin1("WowDisplayName");
}
