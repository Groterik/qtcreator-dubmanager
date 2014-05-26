#include "dubmanager.h"
#include "dubproject.h"

#include "dubprojectmanagerconstants.h"

using namespace DubProjectManager;

DubManager::DubManager(DubOptionsPage *page)
    : optionsPage(page)
{
}

QString DubManager::mimeType() const
{
    return QString::fromUtf8(Constants::DUBMIMETYPE);
}

ProjectExplorer::Project *DubManager::openProject(const QString &fileName, QString *errorString)
{
    if (!QFileInfo(fileName).isFile()) {
        if (errorString)
            *errorString = tr("Failed opening project '%1': Project is not a file")
                .arg(fileName);
        return 0;
    }

    return new DubProject(this, fileName);
}
