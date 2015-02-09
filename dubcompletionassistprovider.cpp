#include "dubcompletionassistprovider.h"

#include "dubprojectmanagerconstants.h"

#include <texteditor/codeassist/keywordscompletionassist.h>

using namespace DubProjectManager;

DubCompletionAssistProvider::DubCompletionAssistProvider()
{

}

DubCompletionAssistProvider::~DubCompletionAssistProvider()
{

}

bool DubCompletionAssistProvider::supportsEditor(Core::Id editorId) const
{
    return editorId == Constants::DUB_EDITOR_ID;
}

TextEditor::IAssistProcessor *DubCompletionAssistProvider::createProcessor() const
{
    TextEditor::Keywords kw;
    return new TextEditor::KeywordsCompletionAssistProcessor(kw);
}

