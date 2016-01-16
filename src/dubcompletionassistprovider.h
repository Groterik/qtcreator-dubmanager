#ifndef DUBCOMPLETIONASSISTPROVIDER_H
#define DUBCOMPLETIONASSISTPROVIDER_H

#include <texteditor/codeassist/completionassistprovider.h>
#include <texteditor/codeassist/keywordscompletionassist.h>

namespace DubProjectManager {

class DubCompletionAssistProvider : public TextEditor::CompletionAssistProvider
{
public:
    DubCompletionAssistProvider();
    ~DubCompletionAssistProvider();

    virtual bool supportsEditor(Core::Id editorId) const Q_DECL_OVERRIDE;
    virtual TextEditor::IAssistProcessor *createProcessor() const Q_DECL_OVERRIDE;
private:
    void initKeywords();
    TextEditor::Keywords m_keywords;
};

class KeywordsWithDetailsCompletionAssistProcessor
        : public TextEditor::KeywordsCompletionAssistProcessor
{
public:
    KeywordsWithDetailsCompletionAssistProcessor(TextEditor::Keywords);

    //virtual TextEditor::IAssistProcessor
    TextEditor::IAssistProposal *perform(
            const TextEditor::AssistInterface *interface) Q_DECL_OVERRIDE;
private:
};

} // namespace DubProjectManager

#endif // DUBCOMPLETIONASSISTPROVIDER_H
