#ifndef DUBMANAGER_H
#define DUBMANAGER_H

#include <projectexplorer/iprojectmanager.h>

class DubManager : public ProjectExplorer::IProjectManager
{
    Q_OBJECT
public:

    // TODO: make settings page
    explicit DubManager(QObject *parent = 0);

    // pure ProjectExplorer::IProjectManager
    virtual QString mimeType() const;
    virtual ProjectExplorer::Project *openProject(const QString &fileName, QString *errorString);

    // others
    
signals:
    
public slots:
    
};

#endif // DUBMANAGER_H
