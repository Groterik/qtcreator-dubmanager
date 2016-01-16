#include "dubrunconfiguration.h"

#include "dubprojectmanagerconstants.h"
#include "dubproject.h"
#include "dubconfigparser.h"

#include <projectexplorer/project.h>
#include <projectexplorer/localenvironmentaspect.h>
#include <utils/pathchooser.h>
#include <utils/detailswidget.h>
#include <coreplugin/coreconstants.h>

#include <QFormLayout>
#include <QLineEdit>
#include <QToolButton>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>

using namespace DubProjectManager;

namespace {
const char RUNCONFIGURATION_ID[] = "DubProjectManager.RunConfiguration";

const char S_ARGUMENTS_KEY[] = "DubProjectManager.DubRunConfiguration.Arguments";
const char S_CONFIGURATION_KEY[] = "DubProjectManager.DubRunConfiguration.Configuration";

} // namespace

using AppMode = ProjectExplorer::ApplicationLauncher::Mode;

DubRunConfiguration::DubRunConfiguration(ProjectExplorer::Target *parent, Core::Id id, DubProject *project)
    : ProjectExplorer::LocalApplicationRunConfiguration(parent, id),
      m_project(project),
      m_runMode(AppMode::Gui)
{
    m_configuration = m_project->configurationList().front();
    init();
}

DubRunConfiguration::DubRunConfiguration(ProjectExplorer::Target *parent, DubRunConfiguration *source)
    : ProjectExplorer::LocalApplicationRunConfiguration(parent, source),
      m_project(source->m_project),
      m_configuration(source->m_configuration),
      m_runMode(source->runMode())
{
    init();
}

QString DubRunConfiguration::executable() const
{
    return m_executable;
}

DubRunConfiguration::RunMode DubRunConfiguration::runMode() const
{
    return m_runMode;
}

QString DubRunConfiguration::workingDirectory() const
{
    return m_workingDirectory;
}

QString DubRunConfiguration::commandLineArguments() const
{
    return m_arguments;
}

QWidget *DubRunConfiguration::createConfigurationWidget()
{
    return new DubRunConfigurationWidget(this);
}

QString DubRunConfiguration::configuration() const
{
    return m_configuration;
}

QStringList DubRunConfiguration::configurationsList() const
{
    return m_project->configurationList();
}

bool DubRunConfiguration::fromMap(const QVariantMap &map)
{
    if (!ProjectExplorer::RunConfiguration::fromMap(map)) {
        return false;
    }
    m_arguments = map.value(QLatin1String(S_ARGUMENTS_KEY)).toString();
    m_configuration = map.value(QLatin1String(S_CONFIGURATION_KEY)).toString();
    update();
    return true;
}

QVariantMap DubRunConfiguration::toMap() const
{
    QVariantMap map(ProjectExplorer::RunConfiguration::toMap());
    map.insert(QLatin1String(S_ARGUMENTS_KEY), m_arguments);
    map.insert(QLatin1String(S_CONFIGURATION_KEY), m_configuration);
    return map;
}

void DubRunConfiguration::setArguments(const QString &args)
{
    m_arguments = args;
    emit updated();
}

void DubRunConfiguration::setConfiguration(const QString &conf)
{
    if (m_configuration == conf || conf.isEmpty()) return;
    m_configuration = conf;
    update();
}

void DubRunConfiguration::runInTerminal(bool toggled)
{
    m_terminal = toggled;
    m_runMode = toggled ? AppMode::Console : AppMode::Gui;
    emit updated();
}

void DubRunConfiguration::update()
{
    m_title = m_project->displayName();
    const ConfigurationInfo &info = m_project->info(m_configuration);
    QDir dpath(info.path());
    m_executable = QDir(dpath.absoluteFilePath(info.targetPath())).absoluteFilePath(info.targetFilename());
    m_workingDirectory = dpath.absoluteFilePath(info.workingDirectory());

    setDefaultDisplayName(displayName());
    setDisplayName(m_title);
    emit updated();
}

void DubRunConfiguration::init()
{
    connect(m_project, SIGNAL(updated()), this, SLOT(update()));
    addExtraAspect(new ProjectExplorer::LocalEnvironmentAspect(this));
    update();
}


DubRunConfigurationWidget::DubRunConfigurationWidget(DubRunConfiguration *dubRunConfiguration, QWidget *parent)
    : QWidget(parent),
      m_dubRunConfiguration(dubRunConfiguration)
{
    QFormLayout *fl = new QFormLayout();
    fl->setMargin(0);
    fl->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

    m_summary = new QLabel(this);
    fl->addRow(tr("<b>Command</b>:"), m_summary);

    m_workingDirectory = new QLabel(this);
    fl->addRow(tr("Working directory:"), m_workingDirectory);

    m_configurations = new QComboBox;
    m_configurations->setDuplicatesEnabled(false);
    connect(m_configurations, SIGNAL(currentTextChanged(QString)),
            m_dubRunConfiguration, SLOT(setConfiguration(QString)));
    fl->addRow(tr("Configuration:"), m_configurations);

    m_argumentsLineEdit = new QLineEdit();
    m_argumentsLineEdit->setText(m_dubRunConfiguration->commandLineArguments());
    connect(m_argumentsLineEdit, SIGNAL(textChanged(QString)), m_dubRunConfiguration, SLOT(setArguments(QString)));
    fl->addRow(tr("Arguments:"), m_argumentsLineEdit);

    ProjectExplorer::EnvironmentAspect *aspect
            = m_dubRunConfiguration->extraAspect<ProjectExplorer::EnvironmentAspect>();
    if (aspect) {
        connect(aspect, SIGNAL(environmentChanged()), this, SLOT(environmentWasChanged()));
        environmentWasChanged();
    }

    m_runInTerminal = new QCheckBox;
    fl->addRow(tr("Run in Terminal"), m_runInTerminal);
    connect(m_runInTerminal, SIGNAL(toggled(bool)),
            m_dubRunConfiguration, SLOT(runInTerminal(bool)));


    m_detailsContainer = new Utils::DetailsWidget(this);
    m_detailsContainer->setState(Utils::DetailsWidget::NoSummary);

    QWidget *m_details = new QWidget(m_detailsContainer);
    m_detailsContainer->setWidget(m_details);
    m_details->setLayout(fl);

    QVBoxLayout *vbx = new QVBoxLayout(this);
    vbx->setMargin(0);
    vbx->addWidget(m_detailsContainer);

    connect(m_dubRunConfiguration, SIGNAL(updated()), this, SLOT(runConfigurationUpdated()));
    runConfigurationUpdated();
}

void DubRunConfigurationWidget::environmentWasChanged()
{
    ProjectExplorer::EnvironmentAspect *aspect
            = m_dubRunConfiguration->extraAspect<ProjectExplorer::EnvironmentAspect>();
    QTC_ASSERT(aspect, return);
}

void DubRunConfigurationWidget::runConfigurationUpdated()
{
    m_configurations->blockSignals(true);
    m_configurations->clear();
    m_configurations->addItems(m_dubRunConfiguration->configurationsList());
    m_configurations->setCurrentText(m_dubRunConfiguration->configuration());
    m_configurations->blockSignals(false);
    if (m_argumentsLineEdit->text() != m_dubRunConfiguration->commandLineArguments()) {
        m_argumentsLineEdit->setText(m_dubRunConfiguration->commandLineArguments());
    }
    if (m_runInTerminal->isChecked() != (m_dubRunConfiguration->runMode() != AppMode::Gui)) {
        m_runInTerminal->setChecked(m_dubRunConfiguration->runMode() != AppMode::Gui);
    }
    m_summary->setText(m_dubRunConfiguration->executable() + " " + m_dubRunConfiguration->commandLineArguments());
    m_workingDirectory->setText(m_dubRunConfiguration->workingDirectory());
}


DubRunConfigurationFactory::DubRunConfigurationFactory(QObject *parent)
    : ProjectExplorer::IRunConfigurationFactory(parent)
{
    setObjectName("DubRunConfigurationFactory");
}

DubRunConfigurationFactory::~DubRunConfigurationFactory()
{

}

bool DubRunConfigurationFactory::canCreate(ProjectExplorer::Target *parent, const Core::Id id) const
{
    Q_UNUSED(id);
    if (!canHandle(parent)) {
        return false;
    }
    return true;
}

bool DubRunConfigurationFactory::canRestore(ProjectExplorer::Target *parent, const QVariantMap &map) const
{
    if (!canHandle(parent)) {
        return false;
    }
    return ProjectExplorer::idFromMap(map).name().startsWith(RUNCONFIGURATION_ID);
}

bool DubRunConfigurationFactory::canClone(ProjectExplorer::Target *parent, ProjectExplorer::RunConfiguration *product) const
{
    if (!canHandle(parent)) {
        return false;
    }
    return product->id().name().startsWith(RUNCONFIGURATION_ID);
}

ProjectExplorer::RunConfiguration *DubRunConfigurationFactory::clone(ProjectExplorer::Target *parent, ProjectExplorer::RunConfiguration *product)
{
    if (!canClone(parent, product)) {
        return 0;
    }
    DubRunConfiguration* w = qobject_cast<DubRunConfiguration*>(product);
    if (!w) {
        return 0;
    }
    return new DubRunConfiguration(parent, w);
}

QList<Core::Id> DubRunConfigurationFactory::availableCreationIds(ProjectExplorer::Target *parent, CreationMode) const
{
    if (!canHandle(parent))
        return QList<Core::Id>();
    QList<Core::Id> allIds;
    QList<ProjectExplorer::Target *> targets = parent->project()->targets();
    foreach (const ProjectExplorer::Target* t, targets) {
        allIds << idFromBuildTarget(t->project()->displayName());
    }
    return allIds;
}

QString DubRunConfigurationFactory::displayNameForId(const Core::Id id) const
{
    return buildTargetFromId(id);
}

Core::Id DubRunConfigurationFactory::idFromBuildTarget(const QString &target)
{
    return Core::Id(RUNCONFIGURATION_ID).withSuffix(target);
}

QString DubRunConfigurationFactory::buildTargetFromId(Core::Id id)
{
    return id.suffixAfter(RUNCONFIGURATION_ID);
}

bool DubRunConfigurationFactory::canHandle(ProjectExplorer::Target *target) const
{
    if (!target) return false;
    if (!target->project()->supportsKit(target->kit()))
        return false;
    return target->project()->id() == DubProjectManager::Constants::DUBPROJECT_ID;
}

ProjectExplorer::RunConfiguration *DubRunConfigurationFactory::doCreate(ProjectExplorer::Target *parent, const Core::Id id)
{
    const QString title(buildTargetFromId(id));
    DubProject* project = qobject_cast<DubProject*>(parent->project());
    return new DubRunConfiguration(parent, id, project);
}

ProjectExplorer::RunConfiguration *DubRunConfigurationFactory::doRestore(ProjectExplorer::Target *parent, const QVariantMap &map)
{
    Q_UNUSED(map);
    return new DubRunConfiguration(parent, DubRunConfigurationFactory::idFromBuildTarget(parent->project()->displayName()),
                                   qobject_cast<DubProject*>(parent->project()));
}
