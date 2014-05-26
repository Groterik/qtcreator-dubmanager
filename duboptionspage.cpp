#include "duboptionspage.h"

#include <projectexplorer/projectexplorerconstants.h>
#include <coreplugin/icore.h>
#include <utils/environment.h>
#include <utils/pathchooser.h>

#include <QCoreApplication>
#include <QSettings>
#include <QFileInfo>
#include <QFormLayout>

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
#if (QT_CREATOR_MAJOR_VERSION == 3)
    m_pathChooser->setHistoryCompleter(QLatin1String("Dub.Command.History"));
#endif
    formLayout->addRow(tr("Executable:"), m_pathChooser);
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
    settings->beginGroup(tr("CMakeSettings"));
    m_dubValidatorForUser->setDubExecutable(settings->value(QLatin1String("dubExecutable")).toString());
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

}

void DubOptionsPage::finish()
{

}

QString DubOptionsPage::findDubExecutable() const
{
    return Utils::Environment::systemEnvironment().searchInPath(QLatin1String("dub"));
}
