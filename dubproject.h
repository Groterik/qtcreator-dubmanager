#ifndef DUBPROJECT_H
#define DUBPROJECT_H

#include <projectexplorer/project.h>

class DubManager;
class DubFile;
class DubProjectNode;

class DubProject : public ProjectExplorer::Project
{
    Q_OBJECT
public:
    explicit DubProject(DubManager* manager, const QString& filename);
    virtual ~DubProject();

    // pure ProjectExplorer::Project
    virtual QString displayName() const;
    virtual Core::Id id() const;
    virtual Core::IDocument *document() const;
    virtual ProjectExplorer::IProjectManager *projectManager() const;

    virtual ProjectExplorer::ProjectNode *rootProjectNode() const;

    // others

    void parseConfig();

    
signals:
    
public slots:

private:

    DubManager* m_manager;
    QString m_filename;
    DubFile* m_file;

    QString m_projectName;
    DubProjectNode *m_rootNode;
    
};

#endif // DUBPROJECT_H
