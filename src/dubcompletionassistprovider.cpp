#include "dubcompletionassistprovider.h"

#include "dubprojectmanagerconstants.h"
#include "dubformatcache.h"

#include <texteditor/codeassist/assistproposalitem.h>
#include <texteditor/codeassist/iassistproposal.h>
#include <texteditor/codeassist/genericproposalmodel.h>
#include <coreplugin/id.h>

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
    return new KeywordsWithDetailsCompletionAssistProcessor(m_keywords);
}

void DubCompletionAssistProvider::initKeywords()
{
    m_keywords = TextEditor::Keywords(DubFormatCache::instance().keywords(),
                                      QStringList(), QMap<QString, QStringList>());
}



KeywordsWithDetailsCompletionAssistProcessor::KeywordsWithDetailsCompletionAssistProcessor(
        TextEditor::Keywords keywords)
    : TextEditor::KeywordsCompletionAssistProcessor(keywords)
{

}

TextEditor::IAssistProposal *KeywordsWithDetailsCompletionAssistProcessor::perform(
        const TextEditor::AssistInterface *interface)
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
        // TODO: set details (doc info for each proposal item).
//        const int size = model->size();
//        for (int i = 0; i < size; ++i) {
//            auto details = DubFormatCache::instance().getKeywordDetails(model->text(i));
//            if (!details.isEmpty()) {
//                model->proposalItem(i)->setDetail(details);
//            }
//        }
    }
    return proposal;
}
