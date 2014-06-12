#ifndef DUBPROJECTNODE_H
#define DUBPROJECTNODE_H

#include <projectexplorer/projectnodes.h>

#if (QT_CREATOR_MAJOR_VERSION == 3)
typedef ProjectExplorer::ProjectAction ProjectAction;
#else
#endif

class DubProjectNode : public ProjectExplorer::ProjectNode
{
    Q_OBJECT
public:
    explicit DubProjectNode(const QString filename);

    // pure ProjectExplorer::ProjectNode

    virtual bool canAddSubProject(const QString &proFilePath) const;

    virtual bool addSubProjects(const QStringList &proFilePaths);
    virtual bool removeSubProjects(const QStringList &proFilePaths);
    virtual QList<ProjectExplorer::RunConfiguration *> runConfigurationsFor(Node *node);

    // others

    bool addFilePath(const QString& path);
    virtual bool addFiles(const QStringList &filePaths, QStringList *notAdded);
    void clear();
    
signals:
    
public slots:

};

#endif // DUBPROJECTNODE_H
