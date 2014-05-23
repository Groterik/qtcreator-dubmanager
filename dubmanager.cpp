#include "dubmanager.h"

#include "dubprojectmanagerconstants.h"

using namespace DubProjectManager;

DubManager::DubManager(QObject *parent)
{
}

QString DubManager::mimeType() const
{
    return QString::fromUtf8(Constants::DUBMIMETYPE);
}

ProjectExplorer::Project *DubManager::openProject(const QString &fileName, QString *errorString)
{
}
