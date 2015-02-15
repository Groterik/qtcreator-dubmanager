#ifndef DUBFILE_H
#define DUBFILE_H

#include <texteditor/textdocument.h>

namespace DubProjectManager {

class DubProject;

class DubFile : public TextEditor::TextDocument
{
    Q_OBJECT
public:
    explicit DubFile(const QString& filename = QString(), DubProject *parent = 0);

    // customize TextEditor::TextDocument
    virtual bool isSaveAsAllowed() const Q_DECL_OVERRIDE;

    // others
    
signals:
    
public slots:

private:
    DubProject* m_project;  
};

} // namespace DubProjectManager

#endif // DUBFILE_H
