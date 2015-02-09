#include "dubprojectmanagerplugin.h"
#include "dubprojectmanagerconstants.h"

#include "dubmanager.h"
#include "duboptionspage.h"
#include "dubbuildconfiguration.h"
#include "dubbuildstep.h"
#include "dubrunconfiguration.h"
#include "dubcompletionassistprovider.h"
#include "dubtexteditorfactory.h"

#include <coreplugin/mimedatabase.h>
#include <coreplugin/icore.h>

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
    if (!Core::MimeDatabase::addMimeTypes(QLatin1String(":resources/DubProject.mimetypes.xml"), errorString))
        return false;
    
    Q_UNUSED(arguments)
    Q_UNUSED(errorString)
    
    DubOptionsPage *optionsPage = new DubOptionsPage;
    addAutoReleasedObject(optionsPage);
    addAutoReleasedObject(new DubManager(optionsPage));
    addAutoReleasedObject(new DubBuildStepFactory);
    addAutoReleasedObject(new DubBuildConfigurationFactory);
    addAutoReleasedObject(new DubRunConfigurationFactory);
    addAutoReleasedObject(new DubCompletionAssistProvider);
    addAutoReleasedObject(new DubTextEditorFactory);
    
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

