#ifndef DUBHOVERHANDLER_H
#define DUBHOVERHANDLER_H

#include <QSharedPointer>

#include <texteditor/basehoverhandler.h>

namespace DubProjectManager {

class DubHoverHandler : public TextEditor::BaseHoverHandler
{
    Q_OBJECT
public:
    explicit DubHoverHandler(QObject *parent = 0);
    virtual ~DubHoverHandler();

    // pure TextEditor::BaseHoverHandler
    virtual void identifyMatch(TextEditor::TextEditorWidget *editor, int pos) Q_DECL_OVERRIDE;

    // overriden
    virtual void decorateToolTip() Q_DECL_OVERRIDE;

signals:

public slots:

private:
    int lastPos;
    QString lastSymbol;
    QString lastTooltip;
};

} // namespace DubProjectManager

#endif // DUBHOVERHANDLER_H
