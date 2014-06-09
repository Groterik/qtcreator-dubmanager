#include "dubbuildstep.h"

#include "dubprojectmanagerconstants.h"
#include "dubproject.h"

#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/target.h>
#include <projectexplorer/buildsteplist.h>
#include <projectexplorer/buildconfiguration.h>

#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>

namespace {
const char BUILDSTEP_ID[] = "DubProjectManager.BuildStep";
const char CLEAN_KEY[] = "DubProjectManager.BuildStep.Clean";
const char BUILD_TARGETS_KEY[] = "DubProjectManager.BuildStep.BuildTargets";
const char ADDITIONAL_ARGUMENTS_KEY[] = "DubProjectManager.BuildStep.AdditionalArguments";

class DubBuildTypes
{
public:
    DubBuildTypes() {
        defaultTypes.push_back(QString::fromLatin1("plain"));
        defaultTypes.push_back(QString::fromLatin1("debug"));
        defaultTypes.push_back(QString::fromLatin1("release"));
        defaultTypes.push_back(QString::fromLatin1("unittest"));
        defaultTypes.push_back(QString::fromLatin1("docs"));
        defaultTypes.push_back(QString::fromLatin1("ddox"));
        defaultTypes.push_back(QString::fromLatin1("profile"));
        defaultTypes.push_back(QString::fromLatin1("cov"));
        defaultTypes.push_back(QString::fromLatin1("unittest-cov"));
    }

    const QString& defaultType() const {
        return defaultTypes.front();
    }
    const QStringList defaultTypesList() const {
        return defaultTypes;
    }
private:
    QStringList defaultTypes;
};

const DubBuildTypes defaultTypes;

}

DubBuildStep::DubBuildStep(ProjectExplorer::BuildStepList *bsl, const Core::Id id) :
    ProjectExplorer::AbstractProcessStep(bsl, id)
{
}

DubBuildStep::DubBuildStep(ProjectExplorer::BuildStepList *bsl, const QString &configurationName)
    : ProjectExplorer::AbstractProcessStep(bsl, BUILDSTEP_ID)
{
    setDisplayName(tr("Dub"));
    m_configuration = configurationName;
    m_buildType = defaultTypes.defaultType();
}

bool DubBuildStep::init()
{
    ProjectExplorer::BuildConfiguration *bc = buildConfiguration();
    if (!bc)
        bc = target()->activeBuildConfiguration();

    ProjectExplorer::ProcessParameters* params = processParameters();
    params->setCommand(command());
    params->setArguments(generateArguments());
    params->setMacroExpander(bc->macroExpander());
    params->setWorkingDirectory(bc->buildDirectory().toString());
    params->setEnvironment(bc->environment());

    setOutputParser(new DubOutputDmdParser);

    return AbstractProcessStep::init();
}

void DubBuildStep::run(QFutureInterface<bool> &fi)
{
    AbstractProcessStep::run(fi);
}

ProjectExplorer::BuildStepConfigWidget *DubBuildStep::createConfigWidget()
{
    return new DubBuildStepConfigWidget(this);
}

const QString &DubBuildStep::additionalArguments() const
{
    return m_additionalArguments;
}

void DubBuildStep::updateConfiguration(const QString &conf)
{
    m_configuration = conf;
    emit updated();
}

void DubBuildStep::updateAdditionalArguments(const QString &args)
{
    m_additionalArguments = args;
    emit updated();
}

void DubBuildStep::updatePackage(const QString &package)
{
    m_package = package;
    emit updated();
}

QString DubBuildStep::generateArguments() const
{
    QString result = "build " + m_package;
    if (!m_configuration.isEmpty()) {
        result += QString::fromLatin1(" --config=") + m_configuration;
    }
    result += QString::fromLatin1(" --build=") + m_buildType;
    result += QString::fromLatin1(" ") + m_additionalArguments;
    return result;
}

const QString &DubBuildStep::configuration() const
{
    return m_configuration;
}

const QString &DubBuildStep::package() const
{
    return m_package;
}

QString DubBuildStep::commandString() const
{
    return command() + " " + generateArguments();
}

QString DubBuildStep::command() const
{
    return QString::fromLatin1("dub");
}

void DubBuildStep::updateBuildType(const QString &type)
{
    m_buildType = type;
    emit updated();
}


DubBuildStepConfigWidget::DubBuildStepConfigWidget(DubBuildStep *step)
    : ProjectExplorer::BuildStepConfigWidget(), m_step(step)
{
    QFormLayout *fl = new QFormLayout(this);
    fl->setMargin(0);
    fl->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    setLayout(fl);

    m_package = new QLineEdit(this);
    fl->addRow(tr("Package:"), m_package);
    m_package->setText(m_step->package());
    connect(m_package, SIGNAL(textChanged(QString)), m_step, SLOT(updatePackage(QString)));

    m_configuration = new QLineEdit(this);
    fl->addRow(tr("Configuration:"), m_configuration);
    m_configuration->setText(m_step->configuration());
    connect(m_configuration, SIGNAL(textChanged(QString)), m_step, SLOT(updateConfiguration(QString)));

    m_additionalArguments = new QLineEdit(this);
    fl->addRow(tr("Additional arguments:"), m_additionalArguments);
    m_additionalArguments->setText(m_step->additionalArguments());
    connect(m_additionalArguments, SIGNAL(textChanged(QString)), m_step, SLOT(updateAdditionalArguments(QString)));

    m_buildTargetsList = new QComboBox(this);
    m_buildTargetsList->addItems(defaultTypes.defaultTypesList());
    m_buildTargetsList->setCurrentText(defaultTypes.defaultType());
    connect(m_buildTargetsList, SIGNAL(currentTextChanged(QString)), m_step, SLOT(updateBuildType(QString)));

    connect(m_step, SIGNAL(updated()), this, SIGNAL(updateSummary()));

    fl->addRow(tr("Build type:"), m_buildTargetsList);
}

QString DubBuildStepConfigWidget::summaryText() const
{
    return QString::fromLatin1("<b>Dub</b>: ") + m_step->commandString();
}

QString DubBuildStepConfigWidget::displayName() const
{
    return QString::fromLatin1("WowDisplayName");
}


DubBuildStepFactory::DubBuildStepFactory(QObject *parent)
    : ProjectExplorer::IBuildStepFactory(parent)
{

}

DubBuildStepFactory::~DubBuildStepFactory()
{

}

bool DubBuildStepFactory::canCreate(ProjectExplorer::BuildStepList *parent, const Core::Id id) const
{
    if (parent->target()->project()->id() == DubProjectManager::Constants::DUBPROJECT_ID
            && parent->id() == ProjectExplorer::Constants::BUILDSTEPS_BUILD)
        return id == BUILDSTEP_ID;
    return false;
}

ProjectExplorer::BuildStep *DubBuildStepFactory::create(ProjectExplorer::BuildStepList *parent, const Core::Id id)
{
    if (!canCreate(parent, id))
        return 0;
    return new DubBuildStep(parent);
}

bool DubBuildStepFactory::canClone(ProjectExplorer::BuildStepList *parent, ProjectExplorer::BuildStep *source) const
{
    return canCreate(parent, source->id());
}

ProjectExplorer::BuildStep *DubBuildStepFactory::clone(ProjectExplorer::BuildStepList *parent, ProjectExplorer::BuildStep *source)
{
    if (!canClone(parent, source))
        return 0;
    return new DubBuildStep(parent, source->id());
}

bool DubBuildStepFactory::canRestore(ProjectExplorer::BuildStepList *parent, const QVariantMap &map) const
{
    Q_UNUSED(parent);
    Q_UNUSED(map);
    return false;
}

ProjectExplorer::BuildStep *DubBuildStepFactory::restore(ProjectExplorer::BuildStepList *parent, const QVariantMap &map)
{
    Q_UNUSED(parent);
    Q_UNUSED(map);
    return 0;
}

QList<Core::Id> DubBuildStepFactory::availableCreationIds(ProjectExplorer::BuildStepList *bc) const
{
    if (bc->target()->project()->id() == DubProjectManager::Constants::DUBPROJECT_ID
            && bc->id() == ProjectExplorer::Constants::BUILDSTEPS_BUILD)
        return QList<Core::Id>() << Core::Id(BUILDSTEP_ID);
    return QList<Core::Id>();
}

QString DubBuildStepFactory::displayNameForId(const Core::Id id) const
{
    if (id == BUILDSTEP_ID) {
        return tr("Dub build step id");
    }
    return QString();
}

const char COMMON_DMD_PATTERN[] = "^(.*)\\((.*)\\): (.*):(.*)$";


DubOutputDmdParser::DubOutputDmdParser()
{
    m_tasks.reserve(MAX_TASKS);
    m_commonDmdError.setPattern(COMMON_DMD_PATTERN);
    m_commonDmdError.setMinimal(true);
}

void DubOutputDmdParser::stdError(const QString &line)
{
    QString trimmedLine = rightTrimmed(line);
    if (trimmedLine.isEmpty()) {
        return;
    }

    if (m_commonDmdError.indexIn(trimmedLine) != -1) {
        ProjectExplorer::Task::TaskType type = m_commonDmdError.cap(3) == "Error" ? ProjectExplorer::Task::Error : ProjectExplorer::Task::Warning;
        m_tasks.push_back(ProjectExplorer::Task(type, m_commonDmdError.cap(4), Utils::FileName::fromUserInput(m_commonDmdError.cap(1)),
                          m_commonDmdError.cap(2).toInt(), ProjectExplorer::Constants::TASK_CATEGORY_BUILDSYSTEM));
    }

    if (m_tasks.size() >= MAX_TASKS) {
        doFlush();
    }

    IOutputParser::stdError(line);
}

void DubOutputDmdParser::doFlush()
{
    foreach (const ProjectExplorer::Task& t, m_tasks) {
        emit addTask(t);
    }
    m_tasks.clear();
}
