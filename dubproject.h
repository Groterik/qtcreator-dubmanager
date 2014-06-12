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

    virtual QStringList files(FilesMode fileMode) const;

    // others

    const QString buildDirectory() const;

    
signals:
    
public slots:

private slots:
    void dubFileChanged(const QString &filename);

private:

    enum TargetType {
        EXECUTABLE, LIBRARY, NONE
    };

    static QStringList scanDirectories(QStringList directories, const QString &root);

    void setupTargets();
    void buildSourceTree();
    void parseConfig();
    void init();

    DubManager* m_manager;
    QString m_filename;
    DubFile* m_file;
    QStringList m_files;
    QStringList m_directories;
    QStringList m_configurations;
    TargetType m_type;

    QString m_projectName;
    DubProjectNode *m_rootNode;
    QFileSystemWatcher* m_watcher;
    QString m_buildDirectory;
    
};

#endif // DUBPROJECT_H
