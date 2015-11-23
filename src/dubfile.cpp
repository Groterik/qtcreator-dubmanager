#include "dubfile.h"

#include "dubprojectmanagerconstants.h"

#include <utils/fileutils.h>

using namespace DubProjectManager;

DubFile::DubFile(const QString &filePath, DubProject *parent) :
    TextEditor::TextDocument(Constants::DUB_EDITOR_ID), m_project(parent)
{
    setMimeType(DubProjectManager::Constants::DUBMIMETYPE);
    setSuggestedFileName(QLatin1String("dub.sdl"));
#if QTCREATOR_MINOR_VERSION < 4
    setFilePath(filePath);
#else
    setFilePath(Utils::FileName::fromString(filePath));
#endif
}

bool DubFile::isSaveAsAllowed() const
{
    return false;
}
