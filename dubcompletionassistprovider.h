#ifndef DUBCOMPLETIONASSISTPROVIDER_H
#define DUBCOMPLETIONASSISTPROVIDER_H

#include <texteditor/codeassist/completionassistprovider.h>

namespace DubProjectManager {

class DubCompletionAssistProvider : public TextEditor::CompletionAssistProvider
{
public:
    DubCompletionAssistProvider();
    ~DubCompletionAssistProvider();

    virtual bool supportsEditor(Core::Id editorId) const Q_DECL_OVERRIDE;
    virtual TextEditor::IAssistProcessor *createProcessor() const Q_DECL_OVERRIDE;
};

} // namespace DubProjectManager

#endif // DUBCOMPLETIONASSISTPROVIDER_H
