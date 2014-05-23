#include "dubprojectnode.h"

DubProjectNode::DubProjectNode(const QString filename) :
    ProjectExplorer::ProjectNode(filename)
{
}


QList<ProjectExplorer::ProjectNode::ProjectAction> DubProjectNode::supportedActions(ProjectExplorer::Node *node) const
{
}

bool DubProjectNode::canAddSubProject(const QString &proFilePath) const
{
}

bool DubProjectNode::addSubProjects(const QStringList &proFilePaths)
{
}


bool DubProjectNode::removeSubProjects(const QStringList &proFilePaths)
{
}

bool DubProjectNode::addFiles(const QStringList &filePaths, QStringList *notAdded)
{
}

bool DubProjectNode::removeFiles(const QStringList &filePaths, QStringList *notRemoved)
{
}

bool DubProjectNode::deleteFiles(const QStringList &filePaths)
{
}

bool DubProjectNode::renameFile(const QString &filePath, const QString &newFilePath)
{
}

QList<ProjectExplorer::RunConfiguration *> DubProjectNode::runConfigurationsFor(ProjectExplorer::Node *node)
{
}
