#include "dubfile.h"

#include "dubprojectmanagerconstants.h"

#include <utils/fileutils.h>

using namespace DubProjectManager;

DubFile::DubFile(const QString &filePath, DubProject *parent)
    : TextEditor::TextDocument(Constants::DUB_EDITOR_ID), m_project(parent)
{
    setMimeType(DubProjectManager::Constants::DUB_MIMETYPE_JSON);
    setFallbackSaveAsFileName(QLatin1String("dub.json"));
    setFilePath(Utils::FileName::fromString(filePath));
}

bool DubFile::isSaveAsAllowed() const
{
    return false;
}
