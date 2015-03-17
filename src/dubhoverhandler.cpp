#include "dubhoverhandler.h"

#include "dubformatcache.h"

#include <texteditor/texteditor.h>

using namespace DubProjectManager;

DubHoverHandler::DubHoverHandler(QObject *parent)
    : lastPos(-1)
{
    Q_UNUSED(parent)
}

DubHoverHandler::~DubHoverHandler()
{

}

int getPos(TextEditor::TextEditorWidget *editor, int pos, int step)
{
    Q_ASSERT(step != 0);
    QChar c = editor->characterAt(pos);
    while (DubFormatCache::isKeywordChar(c)) {
        pos += step;
        c = editor->characterAt(pos);
    }
    return pos;
}

inline QString getKeyword(TextEditor::TextEditorWidget *editor, int pos)
{
    int begin = getPos(editor, pos, -1) + 1;
    int end = getPos(editor, pos, 1);
    return begin >= end ? QString() : editor->textAt(begin, end - begin);
}

void DubHoverHandler::identifyMatch(TextEditor::TextEditorWidget *editor, int pos)
{
    if (!editor) {
        return;
    }
    const TextEditor::TextDocument* doc = editor->textDocument();
    if (!doc) {
        return;
    }
    if (pos != lastPos) {
        lastPos = pos;
        QString kw = getKeyword(editor, pos);
        if (kw.isEmpty()) {
            return;
        }
        if (kw != lastSymbol) {
            lastSymbol = kw;
            lastTooltip = DubFormatCache::instance().getKeywordDetails(kw);
        }

    }
    setToolTip(lastTooltip);
}

void DubHoverHandler::decorateToolTip()
{

}
