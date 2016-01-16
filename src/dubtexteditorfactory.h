#ifndef DUBTEXTEDITORFACTORY_H
#define DUBTEXTEDITORFACTORY_H

#include <texteditor/texteditor.h>

namespace DubProjectManager {

class DubTextEditorFactory : public TextEditor::TextEditorFactory
{
public:
    DubTextEditorFactory();
    virtual ~DubTextEditorFactory();
};

} // namespace DubProjectManager

#endif // DUBTEXTEDITORFACTORY_H
