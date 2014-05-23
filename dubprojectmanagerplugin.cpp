#include "dubprojectmanagerplugin.h"
#include "dubprojectmanagerconstants.h"

#include "dubmanager.h"

#include <coreplugin/icore.h>
#include <coreplugin/icontext.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/coreconstants.h>

#include <QAction>
#include <QMessageBox>
#include <QMainWindow>
#include <QMenu>

#include <QtPlugin>

using namespace DubProjectManager::Internal;

DubProjectManagerPlugin::DubProjectManagerPlugin()
{
    // Create your members
}

DubProjectManagerPlugin::~DubProjectManagerPlugin()
{
    // Unregister objects from the plugin manager's object pool
    // Delete members
}

bool DubProjectManagerPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    // Register objects in the plugin manager's object pool
    
    Q_UNUSED(arguments)
    Q_UNUSED(errorString)
    
    QAction *action = new QAction(tr("DubProjectManager action"), this);
    Core::Command *cmd = Core::ActionManager::registerAction(action, Constants::ACTION_ID,
                                                             Core::Context(Core::Constants::C_GLOBAL));
    cmd->setDefaultKeySequence(QKeySequence(tr("Ctrl+Alt+Meta+A")));
    connect(action, SIGNAL(triggered()), this, SLOT(triggerAction()));
    
    Core::ActionContainer *menu = Core::ActionManager::createMenu(Constants::MENU_ID);
    menu->menu()->setTitle(tr("DubProjectManager"));
    menu->addAction(cmd);
    Core::ActionManager::actionContainer(Core::Constants::M_TOOLS)->addMenu(menu);
    
    return true;
}

void DubProjectManagerPlugin::extensionsInitialized()
{
    // Retrieve objects from the plugin manager's object pool
    // In the extensionsInitialized method, a plugin can be sure that all
    // plugins that depend on it are completely initialized.
}

ExtensionSystem::IPlugin::ShutdownFlag DubProjectManagerPlugin::aboutToShutdown()
{
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)
    return SynchronousShutdown;
}

void DubProjectManagerPlugin::triggerAction()
{
    QMessageBox::information(Core::ICore::mainWindow(),
                             tr("Action triggered"),
                             tr("This is an action from DubProjectManager."));
}

Q_EXPORT_PLUGIN2(DubProjectManager, DubProjectManagerPlugin)

