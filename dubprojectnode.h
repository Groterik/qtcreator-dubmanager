#ifndef DUBPROJECTNODE_H
#define DUBPROJECTNODE_H

#include <projectexplorer/projectnodes.h>

class DubProjectNode : public ProjectExplorer::ProjectNode
{
    Q_OBJECT
public:
    explicit DubProjectNode(const QString filename);

    virtual QList<ProjectAction> supportedActions(Node *node) const;

    virtual bool canAddSubProject(const QString &proFilePath) const;

    virtual bool addSubProjects(const QStringList &proFilePaths);
    virtual bool removeSubProjects(const QStringList &proFilePaths);
    virtual bool addFiles( const QStringList &filePaths,
                          QStringList *notAdded = 0);
    virtual bool removeFiles(const QStringList &filePaths,
                             QStringList *notRemoved = 0);
    virtual bool deleteFiles(const QStringList &filePaths);
    virtual bool renameFile(const QString &filePath,
                            const QString &newFilePath);
    virtual QList<ProjectExplorer::RunConfiguration *> runConfigurationsFor(Node *node);
    
signals:
    
public slots:
    
};

#endif // DUBPROJECTNODE_H
