#include "dubbuildstep.h"

#include "dubprojectmanagerconstants.h"
#include "dubproject.h"
#include "duboptionspage.h"

#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/buildsteplist.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/target.h>

#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QLineEdit>

using namespace DubProjectManager;

namespace {
const char BUILDSTEP_BUILD_ID[] = "DubProjectManager.BuildStep";
const char BUILDSTEP_CLEAN_ID[] = "DubProjectManager.CleanStep";

const char S_PACKAGE_KEY[] = "DubProjectManager.BuildStep.Package";
const char S_BUILD_TYPE_KEY[] = "DubProjectManager.BuildStep.BuildType";
const char S_CONFIGURATION_KEY[] = "DubProjectManager.BuildStep.Configuration";
const char S_ADDITIONAL_ARGUMENTS_KEY[] = "DubProjectManager.BuildStep.AdditionalArguments";
const char S_IS_CLEAN_STEP[] = "DubProjectManager.BuildStep.IsCleanStep";
} // namespace

DubBuildStep::DubBuildStep(ProjectExplorer::BuildStepList *bsl, const Core::Id id) :
    ProjectExplorer::AbstractProcessStep(bsl, id)
{
    construct();
}

DubBuildStep::DubBuildStep(ProjectExplorer::BuildStepList *bsl)
    : ProjectExplorer::AbstractProcessStep(bsl, BUILDSTEP_BUILD_ID)
{
    construct();
    setDisplayName(tr("Dub"));
}

bool DubBuildStep::init(QList<const BuildStep *> &earlierSteps)
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

    setOutputParser(new DubOutputDmdParser(params->workingDirectory()));

    return AbstractProcessStep::init(earlierSteps);
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

void DubBuildStep::makeCleanStep(bool cleanStep)
{
    m_isCleanStep = cleanStep;
    emit updated();
}

void DubBuildStep::construct()
{
    Q_ASSERT(buildConfiguration()->target()->project()->id() == Constants::DUBPROJECT_ID);
    m_project = qobject_cast<DubProject*>(buildConfiguration()->target()->project());

    m_buildType = m_project->buildTypesList().front();
    m_configuration = m_project->configurationList().front();
}

QString DubBuildStep::generateArguments() const
{
    QString result;
    if (m_isCleanStep) {
        result = QLatin1String("clean");
    } else {
        result = QLatin1String("build ") + m_package;
        if (!m_configuration.isEmpty() && m_configuration != Constants::DUB_NO_CONFIG) {
            result += QLatin1String(" --config=\"") + m_configuration + QLatin1String("\"");
        }
        result += QLatin1String(" --build=") + m_buildType;
    }
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

const QString &DubBuildStep::buildType() const
{
    return m_buildType;
}

bool DubBuildStep::isCleanStep() const
{
    return m_isCleanStep;
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
    map.insert(QString::fromLatin1(S_IS_CLEAN_STEP), m_isCleanStep);
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
    m_isCleanStep = map.value(QString::fromLatin1(S_IS_CLEAN_STEP)).toBool();
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

    m_isCleanStep = new QCheckBox(this);
    fl->addRow(tr("Clean step:"), m_isCleanStep);
    m_isCleanStep->setChecked(m_step->isCleanStep());
    connect(m_isCleanStep, SIGNAL(toggled(bool)), m_step, SLOT(makeCleanStep(bool)));

    m_package = new QLineEdit(this);
    fl->addRow(tr("Package:"), m_package);
    m_package->setText(m_step->package());
    connect(m_package, SIGNAL(textChanged(QString)), m_step, SLOT(updatePackage(QString)));

    m_configuration = new QComboBox(this);
    fl->addRow(tr("Configuration:"), m_configuration);
    m_configuration->setCurrentText(m_step->configuration());
    connect(m_configuration, SIGNAL(currentTextChanged(QString)),
            m_step, SLOT(updateConfiguration(QString)));

    m_buildTargetsList = new QComboBox(this);
    fl->addRow(tr("Build type:"), m_buildTargetsList);
    connect(m_buildTargetsList, SIGNAL(currentTextChanged(QString)),
            m_step, SLOT(updateBuildType(QString)));

    m_additionalArguments = new QLineEdit(this);
    fl->addRow(tr("Additional arguments:"), m_additionalArguments);
    m_additionalArguments->setText(m_step->additionalArguments());
    connect(m_additionalArguments, SIGNAL(textChanged(QString)),
            m_step, SLOT(updateAdditionalArguments(QString)));

    connect(m_step, SIGNAL(updated()), this, SLOT(onBuildStepUpdate()));
    connect(m_step, SIGNAL(updated()), this, SIGNAL(updateSummary()));
    connect(m_step->dubProject(), SIGNAL(updated()), this, SLOT(onProjectUpdate()));

    onBuildStepUpdate();
    onProjectUpdate();
}

QString DubBuildStepConfigWidget::summaryText() const
{
    return tr("<b>Dub command</b>: ") + m_step->commandString();
}

QString DubBuildStepConfigWidget::displayName() const
{
    return QString::fromLatin1("WowDisplayName");
}

void DubBuildStepConfigWidget::onBuildStepUpdate()
{
    const bool enableElements = !m_isCleanStep->isChecked();
    m_configuration->setEnabled(enableElements);
    m_package->setEnabled(enableElements);
    m_buildTargetsList->setEnabled(enableElements);
}

void DubBuildStepConfigWidget::onProjectUpdate()
{
    m_buildTargetsList->blockSignals(true);
    m_buildTargetsList->clear();
    m_buildTargetsList->addItems(m_step->dubProject()->buildTypesList());
    m_buildTargetsList->setCurrentText(m_step->buildType());
    m_buildTargetsList->blockSignals(false);

    m_configuration->blockSignals(true);
    m_configuration->clear();
    m_configuration->addItems(m_step->dubProject()->configurationList());
    m_configuration->setCurrentText(m_step->configuration());
    m_configuration->blockSignals(false);
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
    return canHandle(parent) &&
            (id == Core::Id(BUILDSTEP_BUILD_ID) ||
             id == Core::Id(BUILDSTEP_CLEAN_ID));
}

ProjectExplorer::BuildStep *DubBuildStepFactory::create(ProjectExplorer::BuildStepList *parent,
                                                        const Core::Id id)
{
    if (!canCreate(parent, id))
        return 0;
    auto *step = new DubBuildStep(parent);
    if (id == Core::Id(BUILDSTEP_CLEAN_ID)) {
        step->makeCleanStep(true);
    }
    return step;
}

bool DubBuildStepFactory::canClone(ProjectExplorer::BuildStepList *parent,
                                   ProjectExplorer::BuildStep *source) const
{
    return canCreate(parent, source->id());
}

ProjectExplorer::BuildStep *DubBuildStepFactory::clone(ProjectExplorer::BuildStepList *parent,
                                                       ProjectExplorer::BuildStep *source)
{
    if (!canClone(parent, source))
        return 0;
    return new DubBuildStep(parent, source->id());
}

bool DubBuildStepFactory::canRestore(ProjectExplorer::BuildStepList *parent,
                                     const QVariantMap &map) const
{
    return canCreate(parent, ProjectExplorer::idFromMap(map));
}

ProjectExplorer::BuildStep *DubBuildStepFactory::restore(ProjectExplorer::BuildStepList *parent,
                                                         const QVariantMap &map)
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
    if (bc->target()->project()->id() == DubProjectManager::Constants::DUBPROJECT_ID) {
        if (bc->id() == ProjectExplorer::Constants::BUILDSTEPS_BUILD) {
            return QList<Core::Id>() << Core::Id(BUILDSTEP_BUILD_ID);
        }
        if (bc->id() == ProjectExplorer::Constants::BUILDSTEPS_CLEAN) {
            return QList<Core::Id>() << Core::Id(BUILDSTEP_CLEAN_ID);
        }
    }
    return QList<Core::Id>();
}

QString DubBuildStepFactory::displayNameForId(const Core::Id id) const
{
    if (id == BUILDSTEP_BUILD_ID) {
        return tr("Dub Build Step");
    }
    if (id == BUILDSTEP_CLEAN_ID) {
        return tr("Dub Clean Step");
    }
    return QString();
}

bool DubBuildStepFactory::canHandle(ProjectExplorer::BuildStepList *parent) const
{
    return parent->target()->project()->id() == DubProjectManager::Constants::DUBPROJECT_ID
           && (parent->id() == ProjectExplorer::Constants::BUILDSTEPS_BUILD ||
               parent->id() == ProjectExplorer::Constants::BUILDSTEPS_CLEAN);
}

const char COMMON_DMD_PATTERN[] = "^(.*)\\((.*),(.*)\\): (\\w*):(.*)$";


DubOutputDmdParser::DubOutputDmdParser(const QString &workingDir)
    : m_workingDir(workingDir)
{
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
        ProjectExplorer::Task::TaskType type = m_commonDmdError.cap(4) == "Error" ?
                    ProjectExplorer::Task::Error : ProjectExplorer::Task::Warning;
        QDir path = m_commonDmdError.cap(1);
        QString filePath = path.isAbsolute() ? path.path()
                                             : QDir(m_workingDir).filePath(path.path());
        auto task = ProjectExplorer::Task(type, m_commonDmdError.cap(5),
                                          Utils::FileName::fromString(filePath),
                                          m_commonDmdError.cap(2).toInt(),
                                          ProjectExplorer::Constants::TASK_CATEGORY_BUILDSYSTEM);
        emit addTask(task, 1);

    }

    IOutputParser::stdError(line);
}
