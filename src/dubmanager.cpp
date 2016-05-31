#include "dubmanager.h"
#include "dubproject.h"

#include "dubprojectmanagerconstants.h"

#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/projectexplorer.h>
#include <projectexplorer/projectnodes.h>
#include <projectexplorer/projecttree.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <QAction>

using namespace DubProjectManager;

DubManager::DubManager(const QString &mimeType)
    : m_mimeType(mimeType)
{

}

QString DubManager::mimeType() const
{
    return m_mimeType;
}

ProjectExplorer::Project *DubManager::openProject(const QString &fileName, QString *errorString)
{
    try {
        return new DubProject(this, fileName);
    } catch (const std::exception& ex) {
        if (errorString) {
            *errorString = ex.what();
        }
        return 0;
    } catch (...) {
        if (errorString) {
            *errorString = tr("Unknown error during DubManager::openProject");
        }
        return 0;
    }
}

// static
DubManagerFactory &DubManagerFactory::instance()
{
    static DubManagerFactory inst;
    return inst;
}

DubManager *DubManagerFactory::createProjectManager(const QString &mimeType)
{
    return new DubManager(mimeType);
}

void DubManagerFactory::updateProject(ProjectExplorer::Project *project)
{
    DubProject *dubProject = qobject_cast<DubProject*>(project);
    if (dubProject) dubProject->update();
}

void DubManagerFactory::updateContextMenu(ProjectExplorer::Project *project,
                                   ProjectExplorer::Node */*node*/)
{
    m_contextProject = project;
}

DubManagerFactory::DubManagerFactory()
{
    connect(ProjectExplorer::ProjectTree::instance(),
            &ProjectExplorer::ProjectTree::aboutToShowContextMenu,
            this, &DubManagerFactory::updateContextMenu);

    Core::ActionContainer *mproject =
            Core::ActionManager::actionContainer(ProjectExplorer::Constants::M_PROJECTCONTEXT);
    Core::ActionContainer *msubproject =
            Core::ActionManager::actionContainer(ProjectExplorer::Constants::M_SUBPROJECTCONTEXT);

    const Core::Context projectContext(DubProjectManager::Constants::PROJECTCONTEXT);

    QAction *m_updateAction = new QAction(QIcon(), tr("Update Project"), this);
    Core::Command *command = Core::ActionManager::registerAction(
                m_updateAction, DubProjectManager::Constants::DUBUPDATECONTEXTMENU, projectContext);
    command->setAttribute(Core::Command::CA_Hide);
    mproject->addAction(command, ProjectExplorer::Constants::G_PROJECT_BUILD);
    msubproject->addAction(command, ProjectExplorer::Constants::G_PROJECT_BUILD);
    connect(m_updateAction, &QAction::triggered, [this]() {
        updateProject(m_contextProject);
    });
}
