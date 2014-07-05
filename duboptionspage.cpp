#include "duboptionspage.h"

#include <projectexplorer/projectexplorerconstants.h>
#include <coreplugin/icore.h>
#include <utils/environment.h>
#include <utils/pathchooser.h>

#include <QCoreApplication>
#include <QSettings>
#include <QFileInfo>
#include <QFormLayout>

using namespace DubProjectManager;

namespace DubProjectManager {
class DubValidator
{
public:
    DubValidator() : m_state(Invalid) {}
    virtual ~DubValidator() {}

    enum State { Invalid, OK };

    void setDubExecutable(const QString& path);
    QString dubExecutable() const;

    bool isValid() const;

private:
    QString m_executable;
    State m_state;
};
} // namespace DubProjectManager

void DubValidator::setDubExecutable(const QString &path)
{
    m_executable = path;
    QFileInfo info(m_executable);
    if (info.exists() && info.isExecutable()) {
        m_state = OK;
    }
}

QString DubValidator::dubExecutable() const
{
    return m_executable;
}

bool DubValidator::isValid() const
{
    return m_state == OK;
}

DubOptionsWidget::DubOptionsWidget(QWidget *parent)
    : QWidget(parent), m_pathChooser(0)
{
    QFormLayout *formLayout = new QFormLayout(this);
    formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

    m_pathChooser = new Utils::PathChooser(this);
    m_pathChooser->setExpectedKind(Utils::PathChooser::ExistingCommand);
    m_pathChooser->setHistoryCompleter(QLatin1String("Dub.Command.History"));
    formLayout->addRow(tr("Executable:"), m_pathChooser);
}

QString DubOptionsWidget::path() const
{
    return m_pathChooser->path();
}



DubOptionsPage::DubOptionsPage() :
    Core::IOptionsPage(), m_widget(0)
{
    setId("Z.Dub");
    setDisplayName(tr("DubManager"));
    setCategory(ProjectExplorer::Constants::PROJECTEXPLORER_SETTINGS_CATEGORY);
    setDisplayCategory(QCoreApplication::translate("ProjectExplorer",
       ProjectExplorer::Constants::PROJECTEXPLORER_SETTINGS_TR_CATEGORY));
    setCategoryIcon(QLatin1String(ProjectExplorer::Constants::PROJECTEXPLORER_SETTINGS_CATEGORY_ICON));

    m_dubValidatorForSystem = new DubValidator;
    m_dubValidatorForUser = new DubValidator;

    QSettings *settings = Core::ICore::settings();
    settings->beginGroup(tr("DubSettings"));
    m_dubValidatorForUser->setDubExecutable(executable());
    settings->endGroup();

    m_dubValidatorForSystem->setDubExecutable(findDubExecutable());
}

DubOptionsPage::~DubOptionsPage()
{

}

QWidget *DubOptionsPage::widget()
{
    if (!m_widget) {
        m_widget = new DubOptionsWidget();
    }
    return m_widget;
}

void DubOptionsPage::apply()
{
    if (!widget()) {
        return;
    }
    QSettings *settings = Core::ICore::settings();
    settings->beginGroup(QLatin1String("DubSettings"));
    m_dubValidatorForUser->setDubExecutable(m_widget->path());
    settings->setValue(QLatin1String("dubExecutable"), m_dubValidatorForUser->dubExecutable());
    settings->endGroup();
}

void DubOptionsPage::finish()
{
    delete m_widget;
}

QString DubOptionsPage::findDubExecutable() const
{
    return Utils::Environment::systemEnvironment().searchInPath(QLatin1String("dub"));
}

QString DubOptionsPage::executable()
{
    QSettings *settings = Core::ICore::settings();
    settings->beginGroup(tr("DubSettings"));
    QString result = settings->value(QLatin1String("dubExecutable"), QLatin1String("dub")).toString();
    settings->endGroup();
    return result;
}
