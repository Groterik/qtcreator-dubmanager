#ifndef DUBPROJECT_H
#define DUBPROJECT_H

#include <projectexplorer/project.h>

class DubManager;
class DubFile;
class DubProjectNode;
class DubConfigParser;
class ConfigurationInfo;


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

    const QString& buildDirectory() const;

    const QStringList &configurationList() const;
    const QStringList &buildTypesList() const;
    const QString &currentConfiguration() const;

    const ConfigurationInfo &info(const QString conf);

    
signals:
    void updated();
public slots:
    void update();
    void setCurrentConfiguration(const QString& conf);
private slots:
    void dubFileChanged(const QString &filename);

private:

    enum TargetType {
        EXECUTABLE, LIBRARY, NONE
    };

    void setupTargets();
    void parseConfig();
    void buildSourceTree(const QString &conf);
    void init();

    DubManager* m_manager;
    QString m_filename;
    DubFile* m_file;
    QStringList m_files;
    QStringList m_directories;
    QStringList m_configurations;
    QString m_configuration;
    TargetType m_type;

    QString m_projectName;
    DubProjectNode *m_rootNode;
    QFileSystemWatcher* m_watcher;
    QString m_buildDirectory;
    DubConfigParser *m_parser;
    
};

#endif // DUBPROJECT_H
