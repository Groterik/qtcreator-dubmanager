#include "dubbuildstep.h"

#include "dubprojectmanagerconstants.h"
#include "dubproject.h"
#include "duboptionspage.h"

#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/target.h>
#include <projectexplorer/buildsteplist.h>
#include <projectexplorer/buildconfiguration.h>

#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>

using namespace DubProjectManager;

namespace {
const char BUILDSTEP_ID[] = "DubProjectManager.BuildStep";

const char S_PACKAGE_KEY[] = "DubProjectManager.BuildStep.Package";
const char S_BUILD_TYPE_KEY[] = "DubProjectManager.BuildStep.BuildType";
const char S_CONFIGURATION_KEY[] = "DubProjectManager.BuildStep.Configuration";
const char S_ADDITIONAL_ARGUMENTS_KEY[] = "DubProjectManager.BuildStep.AdditionalArguments";
} // namespace

DubBuildStep::DubBuildStep(ProjectExplorer::BuildStepList *bsl, const Core::Id id) :
    ProjectExplorer::AbstractProcessStep(bsl, id)
{
    construct();
}

DubBuildStep::DubBuildStep(ProjectExplorer::BuildStepList *bsl)
    : ProjectExplorer::AbstractProcessStep(bsl, BUILDSTEP_ID)
{
    construct();
    setDisplayName(tr("Dub"));
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

void DubBuildStep::construct()
{
    Q_ASSERT(buildConfiguration()->target()->project()->id() == DubProjectManager::Constants::DUBPROJECT_ID);
    m_project = qobject_cast<DubProject*>(buildConfiguration()->target()->project());

    m_buildType = m_project->buildTypesList().front();
    m_configuration = m_project->configurationList().front();
}

QString DubBuildStep::generateArguments() const
{
    QString result = "build " + m_package;
    if (!m_configuration.isEmpty()) {
        result += QLatin1String(" --config=\"") + m_configuration + QLatin1String("\"");
    }
    result += QLatin1String(" --build=") + m_buildType;
    result += QLatin1String(" ") + m_additionalArguments;
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
    return DubOptionsPage::executable();
}

QVariantMap DubBuildStep::toMap() const
{
    QVariantMap map(ProjectExplorer::AbstractProcessStep::toMap());
    map.insert(QString::fromLatin1(S_PACKAGE_KEY), m_package);
    map.insert(QString::fromLatin1(S_CONFIGURATION_KEY), m_configuration);
    map.insert(QString::fromLatin1(S_BUILD_TYPE_KEY), m_buildType);
    map.insert(QString::fromLatin1(S_ADDITIONAL_ARGUMENTS_KEY), m_additionalArguments);
    return map;
}

const DubProject *DubBuildStep::dubProject() const
{
    return m_project;
}

bool DubBuildStep::fromMap(const QVariantMap &map)
{
    m_package = map.value(QString::fromLatin1(S_PACKAGE_KEY)).toString();
    m_configuration = map.value(QString::fromLatin1(S_CONFIGURATION_KEY)).toString();
    m_buildType = map.value(QString::fromLatin1(S_BUILD_TYPE_KEY)).toString();
    m_additionalArguments = map.value(QString::fromLatin1(S_ADDITIONAL_ARGUMENTS_KEY)).toString();
    emit updated();
    return ProjectExplorer::AbstractProcessStep::fromMap(map);
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
    connect(m_buildTargetsList, SIGNAL(currentTextChanged(QString)), m_step, SLOT(updateBuildType(QString)));

    connect(m_step, SIGNAL(updated()), this, SIGNAL(updateSummary()));
    connect(m_step->dubProject(), SIGNAL(updated()), this, SLOT(update()));

    fl->addRow(tr("Build type:"), m_buildTargetsList);

    update();
}

QString DubBuildStepConfigWidget::summaryText() const
{
    return tr("<b>Dub command</b>: ") + m_step->commandString();
}

QString DubBuildStepConfigWidget::displayName() const
{
    return QString::fromLatin1("WowDisplayName");
}

void DubBuildStepConfigWidget::update()
{
    m_buildTargetsList->blockSignals(true);
    QString oldBuildType = m_buildTargetsList->currentText();
    m_buildTargetsList->clear();
    const QStringList &list = m_step->dubProject()->buildTypesList();
    m_buildTargetsList->addItems(list);
    if (list.contains(oldBuildType)) {
        m_buildTargetsList->setCurrentText(oldBuildType);
    }
    m_buildTargetsList->blockSignals(false);
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
    if (canHandle(parent))
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
    return canCreate(parent, ProjectExplorer::idFromMap(map));
}

ProjectExplorer::BuildStep *DubBuildStepFactory::restore(ProjectExplorer::BuildStepList *parent, const QVariantMap &map)
{
    if (!canRestore(parent, map)) {
        return 0;
    }
    DubBuildStep *bs = new DubBuildStep(parent);
    if (!bs->fromMap(map)) {
        return 0;
    }
    return bs;
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

bool DubBuildStepFactory::canHandle(ProjectExplorer::BuildStepList *parent) const
{
    return parent->target()->project()->id() == DubProjectManager::Constants::DUBPROJECT_ID
           && parent->id() == ProjectExplorer::Constants::BUILDSTEPS_BUILD;
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
