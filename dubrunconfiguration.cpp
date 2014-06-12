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
                                         const QString &/*target*/, const QString &workingDirectory, const QString &title)
    : ProjectExplorer::LocalApplicationRunConfiguration(parent, id),
      m_runMode(Gui),
      m_workingDirectory(workingDirectory),
      m_title(title)
{
    addExtraAspect(new ProjectExplorer::LocalEnvironmentAspect(this));
}

DubRunConfiguration::DubRunConfiguration(ProjectExplorer::Target *parent, ProjectExplorer::LocalApplicationRunConfiguration *source)
    : ProjectExplorer::LocalApplicationRunConfiguration(parent, source),
      m_runMode(source->runMode()),
      m_workingDirectory(source->workingDirectory()),
      m_title(source->displayName())
{

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

void DubRunConfiguration::setArguments(const QString &args)
{
    m_arguments = args;
}

void DubRunConfiguration::setWorkingDirectory(const QString &dir)
{
    m_workingDirectory = dir;
}

void DubRunConfiguration::runInTerminal(bool toggled)
{
    m_terminal = toggled;
}

void DubRunConfiguration::runViaDub(bool toggled)
{
    m_viadub = toggled;
}


DubRunConfigurationWidget::DubRunConfigurationWidget(DubRunConfiguration *cmakeRunConfiguration, QWidget *parent)
    : QWidget(parent)
{
    QFormLayout *fl = new QFormLayout();
    fl->setMargin(0);
    fl->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

    QLineEdit *argumentsLineEdit = new QLineEdit();
    argumentsLineEdit->setText(cmakeRunConfiguration->commandLineArguments());
    connect(argumentsLineEdit, SIGNAL(textChanged(QString)), this, SLOT(setArguments(QString)));
    fl->addRow(tr("Arguments:"), argumentsLineEdit);

    m_workingDirectoryEdit = new Utils::PathChooser();
    m_workingDirectoryEdit->setExpectedKind(Utils::PathChooser::Directory);
    m_workingDirectoryEdit->setBaseDirectory(m_dubRunConfiguration->target()->project()->projectDirectory());
    m_workingDirectoryEdit->setPath(m_dubRunConfiguration->workingDirectory());
    m_workingDirectoryEdit->setHistoryCompleter(QLatin1String("Dub.RunConfiguration.WorkingDirectory.History"));

    ProjectExplorer::EnvironmentAspect *aspect
            = m_dubRunConfiguration->extraAspect<ProjectExplorer::EnvironmentAspect>();
    if (aspect) {
        connect(aspect, SIGNAL(environmentChanged()), this, SLOT(environmentWasChanged()));
        environmentWasChanged();
    }

    m_workingDirectoryEdit->setPromptDialogTitle(tr("Select Working Directory"));
    connect(m_workingDirectoryEdit, SIGNAL(changed(QString)),
            m_dubRunConfiguration, SLOT(setWorkingDirectory(QString)));

    QToolButton *resetButton = new QToolButton();
    resetButton->setToolTip(tr("Reset to default."));
    resetButton->setIcon(QIcon(QLatin1String(Core::Constants::ICON_RESET)));

    QHBoxLayout *boxlayout = new QHBoxLayout();
    boxlayout->addWidget(m_workingDirectoryEdit);
    boxlayout->addWidget(resetButton);

    fl->addRow(tr("Working directory:"), boxlayout);

    QCheckBox *runInTerminal = new QCheckBox;
    fl->addRow(tr("Run in Terminal"), runInTerminal);
    connect(runInTerminal, SIGNAL(toggled(bool)),
            m_dubRunConfiguration, SLOT(runInTerminal(bool)));

    QCheckBox *runViaDub = new QCheckBox;
    fl->addRow(tr("Run via Dub"), runViaDub);
    connect(runInTerminal, SIGNAL(toggled(bool)),
            m_dubRunConfiguration, SLOT(runViaDub(bool)));


    m_detailsContainer = new Utils::DetailsWidget(this);
    m_detailsContainer->setState(Utils::DetailsWidget::NoSummary);

    QWidget *m_details = new QWidget(m_detailsContainer);
    m_detailsContainer->setWidget(m_details);
    m_details->setLayout(fl);

    QVBoxLayout *vbx = new QVBoxLayout(this);
    vbx->setMargin(0);
    vbx->addWidget(m_detailsContainer);
}

void DubRunConfigurationWidget::environmentWasChanged()
{
    ProjectExplorer::EnvironmentAspect *aspect
            = m_dubRunConfiguration->extraAspect<ProjectExplorer::EnvironmentAspect>();
    QTC_ASSERT(aspect, return);
    m_workingDirectoryEdit->setEnvironment(aspect->environment());
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
    if (!canHandle(parent)) {
        return false;
    }
    QList<ProjectExplorer::Target*> tmp = parent->project()->targets();
    Core::Id id2 = tmp.front()->id();
    return parent->project()->target(id) != 0;
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
        allIds << idFromBuildTarget(t->displayName());
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
    const ProjectExplorer::Target *t = parent->project()->target(id);
    return new DubRunConfiguration(parent, id, t->displayName(), "", title);
}

ProjectExplorer::RunConfiguration *DubRunConfigurationFactory::doRestore(ProjectExplorer::Target *parent, const QVariantMap &map)
{
    Q_UNUSED(parent);
    Q_UNUSED(map);
    return 0;
}
