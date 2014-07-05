#ifndef DUBMANAGER_H
#define DUBMANAGER_H

#include <projectexplorer/iprojectmanager.h>

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

private:
    DubOptionsPage *optionsPage;
    
};

} // namespace DubProjectManager

#endif // DUBMANAGER_H
