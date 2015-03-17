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
    addMimeType(Constants::DUBMIMETYPE);

    setDocumentCreator([]() { return new DubProjectManager::DubFile; });
    setEditorWidgetCreator([]() { return new TextEditor::TextEditorWidget; });
    setEditorCreator([]() { return new TextEditor::BaseTextEditor; });
#if QTCREATOR_MINOR_VERSION < 4
    setGenericSyntaxHighlighter(Constants::DUBMIMETYPE);
#else
    setUseGenericHighlighter(true);
#endif

    addHoverHandler(new DubHoverHandler);

    setCompletionAssistProvider(new DubCompletionAssistProvider);

    setParenthesesMatchingEnabled(true);
    setMarksVisible(true);
    setCodeFoldingSupported(true);

}

DubTextEditorFactory::~DubTextEditorFactory()
{

}

