#include "dubtexteditorfactory.h"

#include "dubprojectmanagerconstants.h"

#include "dubcompletionassistprovider.h"

#include <texteditor/textdocument.h>

using namespace DubProjectManager;

DubTextEditorFactory::DubTextEditorFactory()
{
    setId(Constants::DUB_EDITOR_ID);
    setDisplayName(tr(Constants::DUB_EDITOR_DISPLAY_NAME));
    addMimeType(Constants::DUBMIMETYPE);

    setDocumentCreator([]() { return new TextEditor::TextDocument; });
    setEditorWidgetCreator([]() { return new TextEditor::TextEditorWidget; });
    setEditorCreator([]() { return new TextEditor::BaseTextEditor; });

//    setIndenterCreator([]() { return new DlangIndenter; });
//    setAutoCompleterCreator([]() { return new DlangAutoCompleter; });

    setCompletionAssistProvider(new DubCompletionAssistProvider);

//    if (DlangOptionsPage::hoverEnable()) {
//        addHoverHandler(new DlangHoverHandler);
//    }

    setParenthesesMatchingEnabled(true);
    setMarksVisible(true);
    setCodeFoldingSupported(true);

}

DubTextEditorFactory::~DubTextEditorFactory()
{

}

