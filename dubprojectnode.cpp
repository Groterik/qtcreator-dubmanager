#include "dubprojectnode.h"

#include <QDir>

DubProjectNode::DubProjectNode(const QString filename) :
    ProjectExplorer::ProjectNode(filename)
{
}

bool DubProjectNode::canAddSubProject(const QString &proFilePath) const
{
    Q_UNUSED(proFilePath);
    return false;
}

bool DubProjectNode::addSubProjects(const QStringList &proFilePaths)
{
    Q_UNUSED(proFilePaths);
    return false;
}


bool DubProjectNode::removeSubProjects(const QStringList &proFilePaths)
{
    Q_UNUSED(proFilePaths);
    return false;
}

QList<ProjectExplorer::RunConfiguration *> DubProjectNode::runConfigurationsFor(ProjectExplorer::Node *node)
{
    Q_UNUSED(node);
    return QList<ProjectExplorer::RunConfiguration*>();
}

bool DubProjectNode::addFilePath(const QString &path)
{
    QString relativePath = QDir(QFileInfo(this->path()).path()).relativeFilePath(path);
    QStringList subpaths = relativePath.split(QLatin1Char('/'), QString::SkipEmptyParts);
    QString filename = subpaths.last();
    subpaths.pop_back();

    // simply add all structure to node
    ProjectExplorer::FolderNode* node = this;
    foreach (const QString& s, subpaths) {
        ProjectExplorer::FolderNode* added = new ProjectExplorer::FolderNode(s);
        QList<ProjectExplorer::FolderNode*> list;
        list.push_back(added);
        node->addFolderNodes(list);
        node = added;
    }
    node->addFiles(QStringList(filename));

    return true;
}

void DubProjectNode::clear()
{
    this->removeFileNodes(this->fileNodes());
    this->removeFolderNodes(this->subFolderNodes());
}

bool lessNodesByPath(ProjectExplorer::Node *a, ProjectExplorer::Node *b)
{
    return a->path() < b->path();
}

bool mergeProjectNode(ProjectExplorer::FolderNode *root)
{
    if (!root) {
        return false;
    }
    QVector<ProjectExplorer::FolderNode*> folders = root->subFolderNodes().toVector();
    qSort(folders.begin(), folders.end(), lessNodesByPath);
    for (int i = 1; i < folders.size(); ++i) {
        ProjectExplorer::FolderNode *curr = folders[i];
        ProjectExplorer::FolderNode *prev = folders[i - 1];
        if (prev->path() == curr->path()) {
            curr->addFolderNodes(prev->subFolderNodes());
            curr->addFileNodes(prev->fileNodes());
            prev = 0;
        }
    }

    QVector<ProjectExplorer::FolderNode*>::iterator itEnd = std::remove(folders.begin(), folders.end(), static_cast<ProjectExplorer::FolderNode*>(0));
    folders.resize(std::distance(folders.begin(), itEnd));

    foreach (ProjectExplorer::FolderNode* node, folders) {
        mergeProjectNode(node);
    }

    return true;
}
