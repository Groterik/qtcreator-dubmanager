#ifndef DUBMANAGER_H
#define DUBMANAGER_H

#include <projectexplorer/iprojectmanager.h>

namespace ProjectExplorer
{
class Node;
}

namespace DubProjectManager {

class DubManager : public ProjectExplorer::IProjectManager
{
    Q_OBJECT
public:
    // pure ProjectExplorer::IProjectManager
    virtual QString mimeType() const Q_DECL_OVERRIDE;
    virtual ProjectExplorer::Project *openProject(const QString &fileName,
                                                  QString *errorString) Q_DECL_OVERRIDE;
private:
    explicit DubManager(const QString &mimeType);
    QString m_mimeType;
    friend class DubManagerFactory;
};

class DubManagerFactory : public QObject
{
    Q_OBJECT
public:
    static DubManagerFactory& instance();
    DubManager *createProjectManager(const QString &mimeType);
signals:
public slots:
    void updateProject(ProjectExplorer::Project *project);
    void updateContextMenu(ProjectExplorer::Project *project,
                           ProjectExplorer::Node *node);
private:
    DubManagerFactory();
    ProjectExplorer::Project *m_contextProject = nullptr;
};

} // namespace DubProjectManager

#endif // DUBMANAGER_H
