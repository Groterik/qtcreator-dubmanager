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

    explicit DubManager(DubOptionsPage *page, const QString &mimeType);

    // pure ProjectExplorer::IProjectManager
    virtual QString mimeType() const Q_DECL_OVERRIDE;
    virtual ProjectExplorer::Project *openProject(const QString &fileName, QString *errorString) Q_DECL_OVERRIDE;

    // others
    
signals:
    
public slots:
    void updateProject(ProjectExplorer::Project *project);
    void updateContextMenu(ProjectExplorer::Project *project,
                           ProjectExplorer::Node *node);

private:
    DubOptionsPage *optionsPage;
    QString m_mimeType;
    ProjectExplorer::Project *m_contextProject;
    
};

} // namespace DubProjectManager

#endif // DUBMANAGER_H
