#include "dubwizard.h"

#include <QMessageBox>

#include <projectexplorer/projectexplorerconstants.h>

using namespace DubProjectManager;

DubWizard::DubWizard()
{
    setWizardKind(ProjectWizard);
    setCategory(ProjectExplorer::Constants::IMPORT_WIZARD_CATEGORY);
    setDisplayCategory(ProjectExplorer::Constants::IMPORT_WIZARD_CATEGORY_DISPLAY);
    setDescription("Create empty DUB project");
    setDisplayName("DUB Project");
}

void DubWizard::runWizard(const QString &path, QWidget *parent,
                          const QString &platform, const QVariantMap &variables)
{
    Q_UNUSED(path)
    Q_UNUSED(platform)
    Q_UNUSED(variables)
    QMessageBox::information(parent, "Sorry", "Sorry, not implemented yet, please use << dub init >> ");
}
