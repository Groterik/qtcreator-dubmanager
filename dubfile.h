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
    virtual bool save(QString *errorString, const QString &fileName = QString(), bool autoSave = false);
    virtual bool isSaveAsAllowed() const;
    virtual void rename(const QString &newName);

    // others
    
signals:
    
public slots:

private:
    DubProject* m_project;  
};

} // namespace DubProjectManager

#endif // DUBFILE_H
