#include "dubcompletionassistprovider.h"

#include "dubprojectmanagerconstants.h"

#include <texteditor/codeassist/assistproposalitem.h>
#include <texteditor/codeassist/iassistproposal.h>
#include <texteditor/codeassist/genericproposalmodel.h>

using namespace DubProjectManager;

DubCompletionAssistProvider::DubCompletionAssistProvider()
{
    initKeywords();
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
    return new KeywordsWithDetailsCompletionAssistProcessor(m_keywords, m_details);
}

void DubCompletionAssistProvider::initKeywords()
{
    m_keywords = TextEditor::Keywords(m_details.keys(), QStringList(), QMap<QString, QStringList>());
}



KeywordsWithDetailsCompletionAssistProcessor::KeywordsWithDetailsCompletionAssistProcessor(TextEditor::Keywords keywords, QMap<QString, QString> details)
    : TextEditor::KeywordsCompletionAssistProcessor(keywords), m_details(details)
{

}

TextEditor::IAssistProposal *KeywordsWithDetailsCompletionAssistProcessor::perform(const TextEditor::AssistInterface *interface)
{
    TextEditor::IAssistProposal* proposal = KeywordsCompletionAssistProcessor::perform(interface);
    if (proposal) {
        auto imodel = proposal->model();
        if (!imodel) {
            return 0;
        }
        auto model = dynamic_cast<TextEditor::GenericProposalModel*>(imodel);
        if (!model) {
            return 0;
        }
        const int size = model->size();
        for (int i = 0; i < size; ++i) {
            auto it = m_details.find(model->text(i));
            if (it != m_details.end()) {
                model->proposalItem(i)->setDetail(it.value());
            }
        }
    }
    return proposal;
}
