#include "dubprojectnode.h"

#include <QDir>

using namespace DubProjectManager;

DubProjectNode::DubProjectNode(const QString filename)
    : ProjectExplorer::ProjectNode(Utils::FileName::fromString(filename))
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

class NodeEqualPred
{
public:
    NodeEqualPred(const QString& a) : a(a) {}
    bool operator()(ProjectExplorer::FolderNode* b)
    {
        return a == b->filePath().toString();
    }
private:
    const QString& a;
};

bool DubProjectNode::addFilePath(const QString &path)
{
    QString relativePath = QDir(QFileInfo(this->filePath().toString()).path()).relativeFilePath(path);
    QStringList subpaths = relativePath.split(QLatin1Char('/'), QString::SkipEmptyParts);
    subpaths.pop_back();

    // simply add all structure to node
    ProjectExplorer::FolderNode* node = this;
    typedef QList<ProjectExplorer::FolderNode *> SubFolders;
    foreach (const QString& s, subpaths) {
        SubFolders nodes = node->subFolderNodes();
        NodeEqualPred pred(s);
        SubFolders::iterator it = std::find_if(nodes.begin(), nodes.end(), pred);
        if (it == nodes.end()) {
            SubFolders list;
            ProjectExplorer::FolderNode* added = new ProjectExplorer::FolderNode(
                        Utils::FileName::fromString(s));
            list.push_back(added);
            node->addFolderNodes(list);
            node = added;
        } else {
            node = *it;
        }
    }
    node->addFileNodes(QList<ProjectExplorer::FileNode*>()
                       << new ProjectExplorer::FileNode(Utils::FileName::fromString(path),
                                                        ProjectExplorer::SourceType, false));

    return true;
}

bool DubProjectNode::addFiles(const QStringList &filePaths, QStringList *notAdded)
{
    bool result = true;
    if (notAdded) {
        notAdded->clear();
    }
    foreach (const QString& filePath, filePaths) {
        if (!addFilePath(filePath)) {
            result = false;
            if (notAdded) {
                notAdded->push_back(filePath);
            }
        }
    }
    return result;
}

void DubProjectNode::clear()
{
    this->removeFileNodes(this->fileNodes());
    this->removeFolderNodes(this->subFolderNodes());
}
