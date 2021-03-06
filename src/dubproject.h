#ifndef DUBPROJECT_H
#define DUBPROJECT_H

#include <projectexplorer/project.h>

QT_FORWARD_DECLARE_CLASS(QFileSystemWatcher)

namespace DubProjectManager {

class DubManager;
class DubFile;
class DubProjectNode;
class DubConfigParser;
class ConfigurationInfo;

class DubProject : public ProjectExplorer::Project
{
    Q_OBJECT
public:
    explicit DubProject(DubManager* manager, const QString& filename);
    virtual ~DubProject();

    // pure ProjectExplorer::Project
    virtual QString displayName() const Q_DECL_OVERRIDE;
    virtual ProjectExplorer::IProjectManager *projectManager() const Q_DECL_OVERRIDE;

    virtual ProjectExplorer::ProjectNode *rootProjectNode() const Q_DECL_OVERRIDE;

    virtual QStringList files(FilesMode fileMode) const Q_DECL_OVERRIDE;

    // others

    const QString& buildDirectory() const;
    const QStringList &configurationList() const;
    const QStringList &buildTypesList() const;
    const QString &sourceTreeConfiguration() const;

    const ConfigurationInfo &info(const QString conf);

    virtual QVariantMap toMap() const Q_DECL_OVERRIDE;
signals:
    void updated();
public slots:
    void update();
    void setSourceTreeConfiguration(const QString& conf);
protected:
    using RestoreResult = ProjectExplorer::Project::RestoreResult;
    virtual RestoreResult fromMap(const QVariantMap &map, QString *errorMessage) Q_DECL_OVERRIDE;
private slots:
    void dubFileChanged(const QString &filename);

private:

    enum TargetType {
        EXECUTABLE, LIBRARY, NONE
    };

    void setupTargets();
    void parseConfig();
    void buildSourceTree(const QString &conf);
    void updateSourceTree();
    void appendIncludePaths(const ConfigurationInfo &info);
    void init();

    DubManager* m_manager;
    QString m_filename;
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

} // namespace DubProjectManager

#endif // DUBPROJECT_H
