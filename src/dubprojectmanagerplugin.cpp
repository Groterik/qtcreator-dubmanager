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

#if QTCREATOR_MINOR_VERSION < 4
#include <coreplugin/mimedatabase.h>
#else
#include <utils/mimetypes/mimedatabase.h>
#endif

#if QTCREATOR_MINOR_VERSION < 6
#else
#include <coreplugin/iwizardfactory.h>
#endif

#include <coreplugin/icore.h>

#include <QAction>
#include <QMainWindow>
#include <QMenu>

#include <QtPlugin>

using namespace DubProjectManager::Internal;

using namespace Core;
#if QTCREATOR_MINOR_VERSION < 4
#else
using MimeDatabase = ::Utils::MimeDatabase;
#endif

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
#if QTCREATOR_MINOR_VERSION < 4
    if (!Core::MimeDatabase::addMimeTypes(QLatin1String(":resources/DubProject.mimetypes.xml"), errorString))
        return false;
#else
    MimeDatabase::addMimeTypes(QLatin1String(":resources/DubProject.mimetypes.xml"));
#endif
    
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
#if QTCREATOR_MINOR_VERSION < 6
    addAutoReleasedObject(new DubWizard);
#else
    Core::IWizardFactory::registerFactoryCreator([]() {
                return QList<Core::IWizardFactory *>() << new DubWizard;});
#endif
    
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

