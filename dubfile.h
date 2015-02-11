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

    // pure Core::IDocument
    virtual bool save(QString *errorString, const QString &fileName = QString(), bool autoSave = false);

    virtual QString defaultPath() const;
    virtual QString suggestedFileName() const;
    virtual QString mimeType() const;

    virtual bool isModified() const;
    virtual bool isSaveAsAllowed() const;

    virtual bool reload(QString *errorString, ReloadFlag flag, ChangeType type);
    virtual void rename(const QString &newName);

    // others
    
signals:
    
public slots:

private:
    DubProject* m_project;  
};

} // namespace DubProjectManager

#endif // DUBFILE_H
