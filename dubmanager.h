#ifndef DUBMANAGER_H
#define DUBMANAGER_H

#include <projectexplorer/iprojectmanager.h>

namespace ProjectExplorer
{
class Node;
}

namespace DubProjectManager {

class DubOptionsPage;
class DubManager : public ProjectExplorer::IProjectManager
{
    Q_OBJECT
public:

    // TODO: make settings page
    explicit DubManager(DubOptionsPage* page);

    // pure ProjectExplorer::IProjectManager
    virtual QString mimeType() const;
    virtual ProjectExplorer::Project *openProject(const QString &fileName, QString *errorString);

    // others
    
signals:
    
public slots:
    void updateProject(ProjectExplorer::Project *project);
    void updateContextMenu(ProjectExplorer::Project *project,
                           ProjectExplorer::Node *node);

private:
    DubOptionsPage *optionsPage;

    ProjectExplorer::Project *m_contextProject;
    
};

} // namespace DubProjectManager

#endif // DUBMANAGER_H
