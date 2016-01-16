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

DubManager::DubManager(DubOptionsPage *page, const QString &mimeType)
    : optionsPage(page), m_mimeType(mimeType)
{
    connect(ProjectExplorer::ProjectTree::instance(),
            &ProjectExplorer::ProjectTree::aboutToShowContextMenu,
            this, &DubManager::updateContextMenu);

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

void DubManager::updateProject(ProjectExplorer::Project *project)
{
    DubProject *dubProject = qobject_cast<DubProject*>(project);
    if (dubProject) dubProject->update();
}

void DubManager::updateContextMenu(ProjectExplorer::Project *project,
                                   ProjectExplorer::Node */*node*/)
{
    m_contextProject = project;
}
