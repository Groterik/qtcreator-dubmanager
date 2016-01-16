#ifndef DUBPROJECTMANAGER_H
#define DUBPROJECTMANAGER_H

#include "dubprojectmanager_global.h"

#include <extensionsystem/iplugin.h>

namespace DubProjectManager {
namespace Internal {

class DubProjectManagerPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "DubProjectManager.json")
    
public:
    DubProjectManagerPlugin();
    ~DubProjectManagerPlugin();
    
    bool initialize(const QStringList &arguments, QString *errorString) Q_DECL_OVERRIDE;
    void extensionsInitialized() Q_DECL_OVERRIDE;
    ShutdownFlag aboutToShutdown() Q_DECL_OVERRIDE;
    
private slots:
};

} // namespace Internal
} // namespace DubProjectManager

#endif // DUBPROJECTMANAGER_H

