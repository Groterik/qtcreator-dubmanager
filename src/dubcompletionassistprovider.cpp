#include "dubcompletionassistprovider.h"

#include "dubprojectmanagerconstants.h"

#include <texteditor/codeassist/assistproposalitem.h>
#include <texteditor/codeassist/iassistproposal.h>
#include <texteditor/codeassist/genericproposalmodel.h>

#include <QFile>

using namespace DubProjectManager;

static QString wrapByWords(const QString &s, int maxWidth, int minWidth = -1)
{
    const QString BR = QLatin1String("<br></br>");
    QString res;
    res.reserve(s.length());
    int pos = 0;
    const int len = s.length();
    while (true) {
        int lastDelim = pos;
        bool skipped = false;
        int i = pos;
        int num = 0;
        for (; i < len && num < maxWidth; ++i) {
            QChar c = s.at(i);
            if (c == QLatin1Char('<') && !skipped) {
                skipped = true;
            } else if (c == QLatin1Char('>') && skipped) {
                skipped = false;
            } else if (!skipped) {
                if (!c.isLetterOrNumber() && c != QLatin1Char('(')
                        && c != QLatin1Char('[') && c != QLatin1Char('\"')
                        && c != QLatin1Char('\'')) {
                    lastDelim = i;
                }
                ++num;
            }
        }
        if (i == len) {
            res.append(s.midRef(pos, i - pos));
            break;
        }
        if (lastDelim - pos < minWidth) {
            res.append(s.midRef(pos, i - pos));
            res.append(QLatin1Char('-'));
            pos = i;
        } else {
            res.append(s.midRef(pos, lastDelim - pos + 1));
            pos = lastDelim + 1;
        }
        res.append(BR);
    }
    return res;
}

DubCompletionAssistProvider::DubCompletionAssistProvider()
{
    QFile f(QLatin1String(":resources/DubKeywords.html"));
    f.open(QFile::ReadOnly);
    QString html = f.readAll();
    int pos = 0;
    const QString TRB = QLatin1String("<tr>");
    const QString TRE = QLatin1String("</tr>");
    const QString TDB = QLatin1String("<td>");
    const QString TDE = QLatin1String("</td>");
    QString part;
    QString key;
    QString prop;
    QString details;
    while (true) {
        auto trb = html.indexOf(TRB, pos);
        auto tre = html.indexOf(TRE, pos);
        if (trb == -1 || tre == -1)  {
            break;
        }
        pos = tre + TRE.length();
        part.clear();
        part.append(html.midRef(trb, tre - trb));
        auto tdb = part.indexOf(TDB);
        auto tde = part.indexOf(TDE);
        if (tdb == -1 || tde == -1) {
            continue;
        }
        key.clear();
        key.append(part.midRef(tdb + TDB.length(), tde - tdb - TDB.length()));
        prop.clear();
        for (int i = 0; i < key.length(); ++i) {
            QChar c = key.at(i);
            if (!c.isLetterOrNumber()
                    && c != QLatin1Char('_')
                    && c != QLatin1Char('.')
                    && c != QLatin1Char('-')
                    && c != QLatin1Char('\"')
                    && c != QLatin1Char('\'')) {
                prop = key.mid(i);
                key = key.mid(0, i);
                break;
            }
        }
        key.remove(QLatin1Char('\"'));
        key.remove(QLatin1Char('\''));
        part = part.mid(tde + TDE.length());
        part.remove(TDE);
        part.remove(TDB);
        details.clear();
        prop = prop.simplified();
        if (prop.length() > 0) {
            details = QLatin1String("<b>") + prop + QLatin1String("</b><br></br>");
        }
        details += wrapByWords(part.simplified(), 48, 28);
        m_details.insert(key, details);
    }
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
