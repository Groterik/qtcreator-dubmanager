#ifndef DUBPROJECT_H
#define DUBPROJECT_H

#include <projectexplorer/project.h>

class DubManager;
class DubFile;
class DubProjectNode;


QT_FORWARD_DECLARE_CLASS(QFileSystemWatcher)

class DubProject : public ProjectExplorer::Project
{
    Q_OBJECT
public:
    explicit DubProject(DubManager* manager, const QString& filename);
    virtual ~DubProject();

    // pure ProjectExplorer::Project
    virtual QString displayName() const;
    virtual Core::IDocument *document() const;
    virtual ProjectExplorer::IProjectManager *projectManager() const;

    virtual ProjectExplorer::ProjectNode *rootProjectNode() const;

    virtual QStringList files(FilesMode fileMode) const = 0;

    // others

    void parseConfig();

    static QStringList scanDirectories(QStringList directories);

    
signals:
    
public slots:

private slots:
    void dubFileChanged(const QString &filename);

private:

    static void buildTree(DubProjectNode *root, const QStringList &files);

    DubManager* m_manager;
    QString m_filename;
    DubFile* m_file;
    QStringList m_files;

    QString m_projectName;
    DubProjectNode *m_rootNode;
    QFileSystemWatcher* m_watcher;
    
};

#endif // DUBPROJECT_H
