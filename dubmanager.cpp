#include "dubmanager.h"
#include "dubproject.h"

#include "dubprojectmanagerconstants.h"

#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/projectexplorer.h>
#include <projectexplorer/projectnodes.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <QAction>

using namespace DubProjectManager;

DubManager::DubManager(DubOptionsPage *page)
    : optionsPage(page)
{
    ProjectExplorer::ProjectExplorerPlugin *projectExplorer = ProjectExplorer::ProjectExplorerPlugin::instance();
    connect(projectExplorer, &ProjectExplorer::ProjectExplorerPlugin::aboutToShowContextMenu,
            this, &DubManager::updateContextMenu);

    Core::ActionContainer *mproject =
            Core::ActionManager::actionContainer(ProjectExplorer::Constants::M_PROJECTCONTEXT);
    Core::ActionContainer *msubproject =
            Core::ActionManager::actionContainer(ProjectExplorer::Constants::M_SUBPROJECTCONTEXT);

    const Core::Context projectContext(DubProjectManager::Constants::PROJECTCONTEXT);

    QAction *m_updateAction = new QAction(QIcon(), tr("Update Project"), this);
    Core::Command *command = Core::ActionManager::registerAction(m_updateAction,
                                                                 DubProjectManager::Constants::DUBUPDATECONTEXTMENU, projectContext);
    command->setAttribute(Core::Command::CA_Hide);
    mproject->addAction(command, ProjectExplorer::Constants::G_PROJECT_BUILD);
    msubproject->addAction(command, ProjectExplorer::Constants::G_PROJECT_BUILD);
    connect(m_updateAction, &QAction::triggered, [this]() {
        updateProject(m_contextProject);
    });
}

QString DubManager::mimeType() const
{
    return QString::fromUtf8(Constants::DUBMIMETYPE);
}

ProjectExplorer::Project *DubManager::openProject(const QString &fileName, QString *errorString)
{
    if (!QFileInfo(fileName).isFile()) {
        if (errorString)
            *errorString = tr("Failed opening project '%1': Project is not a file")
                .arg(fileName);
        return 0;
    }

        return new DubProject(this, fileName);
    }

void DubManager::updateProject(ProjectExplorer::Project *project)
{
    DubProject *dubProject = qobject_cast<DubProject*>(project);
    if (dubProject) dubProject->update();
}

void DubManager::updateContextMenu(ProjectExplorer::Project *project, ProjectExplorer::Node */*node*/)
{
    m_contextProject = project;
}
