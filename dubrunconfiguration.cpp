#include "dubrunconfiguration.h"

#include "dubprojectmanagerconstants.h"

#include <projectexplorer/project.h>
#include <projectexplorer/localenvironmentaspect.h>
#include <utils/pathchooser.h>
#include <utils/detailswidget.h>
#include <coreplugin/coreconstants.h>

#include <QFormLayout>
#include <QLineEdit>
#include <QToolButton>
#include <QCheckBox>

namespace {
const char RUNCONFIGURATION_ID[] = "DubProjectManager.RunConfiguration";
}

DubRunConfiguration::DubRunConfiguration(ProjectExplorer::Target *parent, Core::Id id,
                                         const QString &executable, const QString &workingDirectory, const QString &title)
    : ProjectExplorer::LocalApplicationRunConfiguration(parent, id),
      m_runMode(Gui),
      m_executable(executable),
      m_workingDirectory(workingDirectory),
      m_defaultWorkingDirectory(workingDirectory),
      m_title(title)
{
    init();
}

DubRunConfiguration::DubRunConfiguration(ProjectExplorer::Target *parent, ProjectExplorer::LocalApplicationRunConfiguration *source)
    : ProjectExplorer::LocalApplicationRunConfiguration(parent, source),
      m_runMode(source->runMode()),
      m_executable(source->executable()),
      m_workingDirectory(source->workingDirectory()),
      m_title(source->displayName())
{
    init();
}

QString DubRunConfiguration::executable() const
{
    return m_executable;
}

ProjectExplorer::LocalApplicationRunConfiguration::RunMode DubRunConfiguration::runMode() const
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

void DubRunConfiguration::resetWorkingDirectory()
{
    m_workingDirectory = m_defaultWorkingDirectory;
    emit updated();
}

void DubRunConfiguration::setArguments(const QString &args)
{
    m_arguments = args;
    emit updated();
}

void DubRunConfiguration::setExecutable(const QString &exec)
{
    m_executable = exec;
    emit updated();
}

void DubRunConfiguration::setWorkingDirectory(const QString &dir)
{
    m_workingDirectory = dir;
    emit updated();
}

void DubRunConfiguration::runInTerminal(bool toggled)
{
    m_terminal = toggled;
    m_runMode = Console;
    emit updated();
}

void DubRunConfiguration::init()
{
    setDefaultDisplayName(displayName());
    setDisplayName(m_title);
    addExtraAspect(new ProjectExplorer::LocalEnvironmentAspect(this));
}


DubRunConfigurationWidget::DubRunConfigurationWidget(DubRunConfiguration *dubRunConfiguration, QWidget *parent)
    : QWidget(parent),
      m_dubRunConfiguration(dubRunConfiguration)
{
    QFormLayout *fl = new QFormLayout();
    fl->setMargin(0);
    fl->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

    m_executableEdit = new Utils::PathChooser;
    m_executableEdit->setExpectedKind(Utils::PathChooser::File);
    m_executableEdit->setBaseDirectory(m_dubRunConfiguration->workingDirectory());
    m_executableEdit->setPath(m_dubRunConfiguration->executable());
    m_executableEdit->setHistoryCompleter(QLatin1String("Dub.RunConfiguration.executable.History"));
    m_executableEdit->setPromptDialogTitle(tr("Select executable"));
    connect(m_executableEdit, SIGNAL(changed(QString)),
            m_dubRunConfiguration, SLOT(setExecutable(QString)));
    fl->addRow(tr("Executable:"), m_executableEdit);

    m_argumentsLineEdit = new QLineEdit();
    m_argumentsLineEdit->setText(m_dubRunConfiguration->commandLineArguments());
    connect(m_argumentsLineEdit, SIGNAL(textChanged(QString)), m_dubRunConfiguration, SLOT(setArguments(QString)));
    fl->addRow(tr("Arguments:"), m_argumentsLineEdit);

    m_workingDirectoryEdit = new Utils::PathChooser;
    m_workingDirectoryEdit->setExpectedKind(Utils::PathChooser::Directory);
    m_workingDirectoryEdit->setBaseDirectory(m_dubRunConfiguration->target()->project()->projectDirectory());
    m_workingDirectoryEdit->setPath(m_dubRunConfiguration->workingDirectory());
    m_workingDirectoryEdit->setHistoryCompleter(QLatin1String("Dub.RunConfiguration.WorkingDirectory.History"));
    m_workingDirectoryEdit->setPromptDialogTitle(tr("Select Working Directory"));
    connect(m_workingDirectoryEdit, SIGNAL(changed(QString)),
            m_dubRunConfiguration, SLOT(setWorkingDirectory(QString)));

    ProjectExplorer::EnvironmentAspect *aspect
            = m_dubRunConfiguration->extraAspect<ProjectExplorer::EnvironmentAspect>();
    if (aspect) {
        connect(aspect, SIGNAL(environmentChanged()), this, SLOT(environmentWasChanged()));
        environmentWasChanged();
    }

    QToolButton *resetButton = new QToolButton();
    resetButton->setToolTip(tr("Reset to default."));
    resetButton->setIcon(QIcon(QLatin1String(Core::Constants::ICON_RESET)));
    connect(resetButton, SIGNAL(clicked()),
            m_dubRunConfiguration, SLOT(resetWorkingDirectory()));

    QHBoxLayout *boxlayout = new QHBoxLayout();
    boxlayout->addWidget(m_workingDirectoryEdit);
    boxlayout->addWidget(resetButton);

    fl->addRow(tr("Working directory:"), boxlayout);

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
}

void DubRunConfigurationWidget::environmentWasChanged()
{
    ProjectExplorer::EnvironmentAspect *aspect
            = m_dubRunConfiguration->extraAspect<ProjectExplorer::EnvironmentAspect>();
    QTC_ASSERT(aspect, return);
    m_workingDirectoryEdit->setEnvironment(aspect->environment());
}

void DubRunConfigurationWidget::runConfigurationUpdated()
{
    m_workingDirectoryEdit->setPath(m_dubRunConfiguration->workingDirectory());
    m_executableEdit->setPath(m_dubRunConfiguration->executable());
    m_argumentsLineEdit->setText(m_dubRunConfiguration->commandLineArguments());
    m_runInTerminal->setChecked(m_dubRunConfiguration->runMode() != DubRunConfiguration::Gui);
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
    Q_UNUSED(parent);
    Q_UNUSED(map);
    return false;
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
    ProjectExplorer::LocalApplicationRunConfiguration* w = dynamic_cast<ProjectExplorer::LocalApplicationRunConfiguration*>(product);
    if (!w) {
        return 0;
    }
    return new DubRunConfiguration(parent, w);
}

QList<Core::Id> DubRunConfigurationFactory::availableCreationIds(ProjectExplorer::Target *parent) const
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
    return new DubRunConfiguration(parent, id, parent->project()->displayName(), parent->project()->projectDirectory(), title);
}

ProjectExplorer::RunConfiguration *DubRunConfigurationFactory::doRestore(ProjectExplorer::Target *parent, const QVariantMap &map)
{
    Q_UNUSED(parent);
    Q_UNUSED(map);
    return 0;
}
