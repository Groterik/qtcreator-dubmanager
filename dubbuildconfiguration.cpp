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
#include <coreplugin/coreconstants.h>

#include <QFormLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QCheckBox>
#include <QToolButton>
#include <QPushButton>

using namespace DubProjectManager;

DubBuildConfiguration::DubBuildConfiguration(ProjectExplorer::Target *target, BuildConfiguration *source) :
    ProjectExplorer::BuildConfiguration(target, source),
    m_project(qobject_cast<DubProject*>(target->project()))
{
    Q_ASSERT(m_project);
    Q_UNUSED(target);
    cloneSteps(source);
}

DubBuildConfiguration::DubBuildConfiguration(ProjectExplorer::Target *target, const Core::Id id)
    : ProjectExplorer::BuildConfiguration(target, id),
      m_project(qobject_cast<DubProject*>(target->project()))
{
    Q_ASSERT(m_project);
}

DubBuildConfiguration::DubBuildConfiguration(ProjectExplorer::Target *target, const ProjectExplorer::BuildInfo &info)
    : ProjectExplorer::BuildConfiguration(target, DubProjectManager::Constants::DUB_BC_ID),
      m_project(qobject_cast<DubProject*>(target->project()))
{
    Q_ASSERT(m_project);
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

DubProject &DubBuildConfiguration::dubPoject()
{
    return *m_project;
}


DubBuildConfigurationWidget::DubBuildConfigurationWidget(DubBuildConfiguration* configuration)
    : m_configuration(configuration), m_ccw(" for project ", this)
{
    this->setDisplayName("DubManager");

    QFormLayout *fl = new QFormLayout(this);
    fl->setMargin(0);
    fl->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

    QToolButton *resetButton = new QToolButton();
    resetButton->setToolTip(tr("Update project"));
    resetButton->setIcon(QIcon(QLatin1String(Core::Constants::ICON_RESET)));
    connect(resetButton, SIGNAL(clicked()), &(m_configuration->dubPoject()), SLOT(update()));
    fl->addRow(tr("Update project:"), resetButton);

    fl->addRow(tr("Source tree: "), &m_ccw);

    connect(&(m_configuration->dubPoject()), SIGNAL(updated()), this, SLOT(update()));
    connect(&m_ccw, SIGNAL(currentTextChanged(QString)), &(m_configuration->dubPoject()), SLOT(setSourceTreeConfiguration(QString)));

    update();

}

void DubBuildConfigurationWidget::update()
{
    m_ccw.setCurrentText(m_configuration->dubPoject().sourceTreeConfiguration(), m_configuration->dubPoject().configurationList());
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

CheckedComboBoxWidget::CheckedComboBoxWidget(const QString &label, QWidget *parent)
    : QWidget(parent)
{
    m_check = new QCheckBox(label, this);
    m_check->setChecked(false);

    m_combo = new QComboBox(this);
    m_combo->setDuplicatesEnabled(false);
    m_combo->setEnabled(false);

    m_apply = new QPushButton(tr("Apply"));

    m_cancel = new QPushButton(tr("Cancel"));

    QHBoxLayout *hl = new QHBoxLayout(this);
    hl->addWidget(m_check);
    hl->addWidget(m_combo);
    hl->addWidget(m_apply);
    hl->addWidget(m_cancel);
    this->setLayout(hl);

    connect(m_check, SIGNAL(toggled(bool)), this, SLOT(chooseText()));
    connect(m_combo, SIGNAL(currentTextChanged(QString)), this, SLOT(chooseText()));
    connect(m_check, SIGNAL(toggled(bool)), m_combo, SLOT(setEnabled(bool)));
    connect(m_apply, SIGNAL(clicked()), this, SLOT(onApply()));
    connect(m_cancel, SIGNAL(clicked()), this, SLOT(onCancel()));
}

void CheckedComboBoxWidget::setLabel(const QString &label)
{
    m_check->setText(label);
}

void CheckedComboBoxWidget::setCurrentText(const QString &text, const QStringList &list)
{
    m_combo->clear();
    m_combo->addItems(list);
    setCurrentText(text);
}

QString CheckedComboBoxWidget::currentText() const
{
    return m_current;
}

QString CheckedComboBoxWidget::newText() const
{
    return m_check->isChecked() ? m_combo->currentText() : QString();
}

void CheckedComboBoxWidget::setCurrentText(QString text)
{
    m_current = text;
    if (!text.isEmpty()) {
        m_combo->setCurrentText(text);
    }
    m_check->setChecked(!text.isEmpty());
    chooseText();
}

void CheckedComboBoxWidget::chooseText()
{
    bool isChanged = (m_current != newText());
    m_apply->setEnabled(isChanged);
    m_cancel->setEnabled(isChanged);
}

void CheckedComboBoxWidget::onApply()
{
    QString s = newText();
    setCurrentText(s);
    emit currentTextChanged(s);
}

void CheckedComboBoxWidget::onCancel()
{
    setCurrentText(m_current);
}
