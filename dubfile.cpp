#include "dubfile.h"

#include "dubprojectmanagerconstants.h"

using namespace DubProjectManager;

DubFile::DubFile(const QString &filePath, DubProject *parent) :
    Core::IDocument(), m_project(parent)
{
    setFilePath(filePath);
}

bool DubFile::save(QString *errorString, const QString &fileName, bool autoSave)
{
    Q_UNUSED(errorString);
    Q_UNUSED(fileName);
    Q_UNUSED(autoSave);
    return false;
}

QString DubFile::defaultPath() const
{
    return QString();
}

QString DubFile::suggestedFileName() const
{
    return QString::fromUtf8("dub.json");
}

QString DubFile::mimeType() const
{
    return QString::fromUtf8(DubProjectManager::Constants::DUBMIMETYPE);
}

bool DubFile::isModified() const
{
    return false;
}

bool DubFile::isSaveAsAllowed() const
{
    return false;
}

bool DubFile::reload(QString *errorString, Core::IDocument::ReloadFlag flag, Core::IDocument::ChangeType type)
{
    Q_UNUSED(errorString);
    Q_UNUSED(flag);
    Q_UNUSED(type);
    return false;
}

void DubFile::rename(const QString &newName)
{
    Q_UNUSED(newName);
}
