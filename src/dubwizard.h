#ifndef DUBWIZARD_H
#define DUBWIZARD_H

#include <coreplugin/iwizardfactory.h>
#include <utils/wizard.h>

#include <QSharedPointer>

namespace Utils {
class PathChooser;
}

QT_FORWARD_DECLARE_CLASS(QLabel)
QT_FORWARD_DECLARE_CLASS(QComboBox)
QT_FORWARD_DECLARE_CLASS(QLineEdit)
QT_FORWARD_DECLARE_CLASS(QPlainTextEdit)
QT_FORWARD_DECLARE_CLASS(QProcess)


namespace DubProjectManager {

class DubWizard : public Core::IWizardFactory
{
    Q_OBJECT
public:
    DubWizard();

    virtual void runWizard(const QString &path, QWidget *parent,
                           const QString &platform, const QVariantMap &variables) Q_DECL_OVERRIDE;

    QString buildDirectory() const;
};

class DubWizardWidget : public Utils::Wizard
{
    Q_OBJECT
public:
    DubWizardWidget();
    void setDubCommand(const QString& cmd);
    void setDubArguments(const QStringList& args);
    QString dubCommand() const;
    QStringList dubArguments() const;

    void setDirectory(const QString& dir);
    QString directory() const;
private:
    QString m_dubCommand;
    QStringList m_dubArguments;
    QString m_directory;
};

class DubInitSettingsPage : public QWizardPage
{
    Q_OBJECT
public:
    explicit DubInitSettingsPage(DubWizardWidget *parent);

    virtual bool validatePage() Q_DECL_OVERRIDE;
    virtual bool isComplete() const Q_DECL_OVERRIDE;
private slots:
    void updateDubCommand();
private:
    QStringList getArguments() const;
    QString getCommand() const;
private:
    DubWizardWidget *m_wizardWidget;
    Utils::PathChooser *m_pc;
    QLabel *m_dubCommand;
    QLineEdit *m_dubInitExtraArguments;
    QComboBox *m_dubInitType;
};

class DubInitResultsPage : public QWizardPage
{
    Q_OBJECT
public:
    explicit DubInitResultsPage(DubWizardWidget *parent);

    virtual void initializePage() Q_DECL_OVERRIDE;
    virtual bool validatePage() Q_DECL_OVERRIDE;
    virtual bool isComplete() const Q_DECL_OVERRIDE;
    virtual void cleanupPage() Q_DECL_OVERRIDE;
private slots:
    void dubReadyReadStandardOutput();
    void dubReadyReadStandardError();
    void dubFinished();
    void setCompleted(bool b);
private:
    DubWizardWidget *m_wizardWidget;
    QPlainTextEdit *m_output;
    QLabel* m_exitCode;
    QSharedPointer<QProcess> m_dubProcess;
    bool m_isCompleted;
};

} // namespace DubProjectManager

#endif // DUBWIZARD_H
