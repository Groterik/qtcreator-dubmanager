#include "dubprojectmanagerplugin.h"
#include "dubprojectmanagerconstants.h"

#include "dubmanager.h"
#include "duboptionspage.h"
#include "dubbuildconfiguration.h"
#include "dubbuildstep.h"
#include "dubrunconfiguration.h"
#include "dubcompletionassistprovider.h"
#include "dubtexteditorfactory.h"
#include "dubwizard.h"

#include <utils/mimetypes/mimedatabase.h>
#include <coreplugin/iwizardfactory.h>
#include <coreplugin/icore.h>

#include <QAction>
#include <QMainWindow>
#include <QMenu>

#include <QtPlugin>

using namespace DubProjectManager::Internal;

using namespace Core;
using MimeDatabase = ::Utils::MimeDatabase;

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
    MimeDatabase::addMimeTypes(QLatin1String(":resources/DubProject.mimetypes.xml"));
    
    Q_UNUSED(arguments)
    Q_UNUSED(errorString)
    
    DubOptionsPage *optionsPage = new DubOptionsPage;
    addAutoReleasedObject(optionsPage);
    addAutoReleasedObject(new DubManager(optionsPage, Constants::DUB_MIMETYPE_JSON));
    addAutoReleasedObject(new DubManager(optionsPage, Constants::DUB_MIMETYPE_SDL));
    addAutoReleasedObject(new DubBuildStepFactory);
    addAutoReleasedObject(new DubBuildConfigurationFactory);
    addAutoReleasedObject(new DubRunConfigurationFactory);
    addAutoReleasedObject(new DubCompletionAssistProvider);
    addAutoReleasedObject(new DubTextEditorFactory);
    Core::IWizardFactory::registerFactoryCreator([]() {
                return QList<Core::IWizardFactory *>() << new DubWizard;});
    
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

