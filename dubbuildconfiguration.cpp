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
    : m_configuration(configuration)
{
    this->setDisplayName("DubManager");

    QFormLayout *fl = new QFormLayout(this);
    fl->setMargin(0);
    fl->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

    QToolButton *resetButton = new QToolButton();
    resetButton->setToolTip(tr("Reset to default"));
    resetButton->setIcon(QIcon(QLatin1String(Core::Constants::ICON_RESET)));
    connect(resetButton, SIGNAL(clicked()), &(m_configuration->dubPoject()), SLOT(update()));

    m_chooseConfigs = new QCheckBox(tr(" for config "), this);
    m_configs = new QComboBox(this);
    m_configs->setDuplicatesEnabled(false);
    connect(m_chooseConfigs, SIGNAL(toggled(bool)), this, SLOT(chooseConfiguration()));
    m_configs->setEnabled(false);
    connect(m_configs, SIGNAL(currentTextChanged(QString)), this, SLOT(chooseConfiguration()));

    QHBoxLayout *hl = new QHBoxLayout;
    hl->addWidget(m_chooseConfigs);
    hl->addWidget(m_configs);
    hl->addWidget(resetButton, 1, Qt::AlignRight);

    fl->addRow(tr("Source tree: "), hl);

    connect(&(m_configuration->dubPoject()), SIGNAL(updated()), this, SLOT(update()));
    connect(this, SIGNAL(configurationChoosed(QString)), &(m_configuration->dubPoject()), SLOT(setCurrentConfiguration(QString)));

    update();

}

void DubBuildConfigurationWidget::update()
{
    QString currentConfig = m_configuration->dubPoject().currentConfiguration();
    m_configs->clear();
    m_chooseConfigs->setChecked(false);
    if (!currentConfig.isEmpty()) {
        m_configs->addItem(currentConfig);
    }
    m_configs->addItems(m_configuration->dubPoject().configurationList());
    if (currentConfig.isEmpty()) {
        m_configs->setEnabled(false);
        m_chooseConfigs->setChecked(false);
    } else {
        m_configs->setEnabled(true);
        m_chooseConfigs->setChecked(true);
        m_configs->setCurrentText(currentConfig);
    }
}

void DubBuildConfigurationWidget::chooseConfiguration()
{
    bool enable = m_chooseConfigs->isChecked();
    m_configs->setEnabled(enable);
    if (enable) {
        emit configurationChoosed(m_configs->currentText());
    } else {
        emit configurationChoosed(QString());
    }
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
