#ifndef DUBFILE_H
#define DUBFILE_H

#include <coreplugin/idocument.h>

class DubProject;

class DubFile : public Core::IDocument
{
    Q_OBJECT
public:
    explicit DubFile(const QString& filename, DubProject *parent = 0);

    // pure Core::IDocument
    virtual bool save(QString *errorString, const QString &fileName = QString(), bool autoSave = false);
    virtual QString fileName() const;

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
    QString m_filename;
    
};

#endif // DUBFILE_H
