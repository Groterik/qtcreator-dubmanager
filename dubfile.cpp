#include "dubfile.h"

#include "dubprojectmanagerconstants.h"

using namespace DubProjectManager;

DubFile::DubFile(const QString &filePath, DubProject *parent) :
    TextEditor::TextDocument(Constants::DUB_EDITOR_ID), m_project(parent)
{
    setMimeType(DubProjectManager::Constants::DUBMIMETYPE);
    setSuggestedFileName(QLatin1String("dub.json"));
    setFilePath(filePath);
}

bool DubFile::save(QString *errorString, const QString &fileName, bool autoSave)
{
    Q_UNUSED(errorString);
    Q_UNUSED(fileName);
    Q_UNUSED(autoSave);
    return false;
}

bool DubFile::isSaveAsAllowed() const
{
    return false;
}

void DubFile::rename(const QString &newName)
{
    Q_UNUSED(newName);
}
