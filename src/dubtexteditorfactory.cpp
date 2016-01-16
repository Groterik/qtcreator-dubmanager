#include "dubtexteditorfactory.h"

#include "dubprojectmanagerconstants.h"

#include "dubcompletionassistprovider.h"
#include "dubfile.h"
#include "dubhoverhandler.h"

using namespace DubProjectManager;

DubTextEditorFactory::DubTextEditorFactory()
{
    setId(Constants::DUB_EDITOR_ID);
    setDisplayName(tr(Constants::DUB_EDITOR_DISPLAY_NAME));
    addMimeType(Constants::DUB_MIMETYPE_JSON);

    setDocumentCreator([]() { return new DubProjectManager::DubFile; });
    setEditorWidgetCreator([]() { return new TextEditor::TextEditorWidget; });
    setEditorCreator([]() { return new TextEditor::BaseTextEditor; });
    setUseGenericHighlighter(true);

    addHoverHandler(new DubHoverHandler);

    setCompletionAssistProvider(new DubCompletionAssistProvider);

    setParenthesesMatchingEnabled(true);
    setMarksVisible(true);
    setCodeFoldingSupported(true);

}

DubTextEditorFactory::~DubTextEditorFactory()
{

}

