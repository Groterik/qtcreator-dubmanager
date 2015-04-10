#include "dubwizard.h"

#include "duboptionspage.h"

#include <projectexplorer/projectexplorerconstants.h>
#include <utils/pathchooser.h>
#include <texteditor/fontsettings.h>

#include <QFormLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QProcess>
#include <QMessageBox>

const char CATEGORY[] = "I.Projects";
const char DISPLAY_CATEGORY[] = QT_TRANSLATE_NOOP("DubWizard", "Non-Qt Project");
const QString DUB_INIT_NO_TYPE = QLatin1String("<no type>");

using namespace DubProjectManager;

DubWizard::DubWizard()
{
    setWizardKind(ProjectWizard);
    setCategory(CATEGORY);
    setDisplayCategory(DISPLAY_CATEGORY);
    setDescription("Create empty DUB project");
    setDisplayName("DUB Project");
    setFlags(PlatformIndependent);
}

void DubWizard::runWizard(const QString &path, QWidget *parent,
                          const QString &platform, const QVariantMap &variables)
{
    Q_UNUSED(path)
    Q_UNUSED(platform)
    Q_UNUSED(variables)

    QMessageBox::information(parent, "Sorry", "Sorry, not implemented yet, please use << dub init >> ");
}


DubWizardWidget::DubWizardWidget()
{
    setWindowTitle(tr("Init DUB project"));

    addPage(new DubInitSettingsPage(this));
    addPage(new DubInitResultsPage(this));
}

void DubWizardWidget::setDubCommand(const QString &cmd)
{
    m_dubCommand = cmd;
}

void DubWizardWidget::setDubArguments(const QStringList &args)
{
    m_dubArguments = args;
}

QString DubWizardWidget::dubCommand() const
{
    return m_dubCommand;
}

QStringList DubWizardWidget::dubArguments() const
{
    return m_dubArguments;
}

void DubWizardWidget::setDirectory(const QString &dir)
{
    m_directory = dir;
}

DubInitSettingsPage::DubInitSettingsPage(DubWizardWidget *parent)
    : m_wizardWidget(parent)
{
    auto layout = new QFormLayout(this);
    m_pc = new Utils::PathChooser;
    layout->addRow("Directory: ", m_pc);
    connect(m_pc, &Utils::PathChooser::pathChanged, [this] {
        this->updateDubCommand();
    });

    m_dubCommand = new QLabel;
    layout->addRow("DUB command: ", m_dubCommand);

    m_dubInitType = new QComboBox(this);
    connect(m_dubInitType, &QComboBox::currentTextChanged, [this]() {
        this->updateDubCommand();
    });
    m_dubInitType->addItems(QStringList() << DUB_INIT_NO_TYPE << "minimal" << "vibe.d");

    m_dubInitExtraArguments = new QLineEdit;
    connect(m_dubInitExtraArguments, &QLineEdit::textChanged, [this]() {
        this->updateDubCommand();
    });

    setLayout(layout);

    setTitle(tr("Init DUB project"));
}

bool DubInitSettingsPage::validatePage()
{
    m_wizardWidget->setDubCommand(getCommand());
    m_wizardWidget->setDubArguments(getArguments());
    m_wizardWidget->setDirectory(m_pc->path());
    return true;
}


void DubInitSettingsPage::updateDubCommand()
{
    QChar delim = QLatin1Char(' ');
    m_dubCommand->setText(getCommand() + delim + getArguments().join(delim));
}

QStringList DubInitSettingsPage::getArguments() const
{
    QStringList result;
    result << QLatin1String("init") << m_pc->path();
    auto type = m_dubInitType->currentText();
    if (type != DUB_INIT_NO_TYPE) {
        result << type;
    }
    result << m_dubInitExtraArguments->text();
    return result;
}

QString DubInitSettingsPage::getCommand() const
{
    return DubOptionsPage::executable();
}


DubInitResultsPage::DubInitResultsPage(DubWizardWidget *parent)
    : m_wizardWidget(parent), m_isCompleted(false)
{
    auto layout = new QFormLayout;

    m_output = new QPlainTextEdit;
    m_output = new QPlainTextEdit(this);
    m_output->setReadOnly(true);
    m_output->setMinimumHeight(15);
    QFont f(TextEditor::FontSettings::defaultFixedFontFamily());
    f.setStyleHint(QFont::TypeWriter);
    m_output->setFont(f);
    QSizePolicy pl = m_output->sizePolicy();
    pl.setVerticalStretch(1);
    m_output->setSizePolicy(pl);
    layout->addRow(m_output);

    m_exitCode = new QLabel(this);
    m_exitCode->setVisible(false);
    layout->addRow(m_exitCode);
}

void DubInitResultsPage::initializePage()
{
    setCompleted(false);
    m_dubProcess.reset();
    m_dubProcess = QSharedPointer<QProcess>(new QProcess);
    m_dubProcess->setProgram(m_wizardWidget->dubCommand());
    m_dubProcess->setArguments(m_wizardWidget->dubArguments());
    connect(m_dubProcess.data(), SIGNAL(readyReadStandardOutput()), this, SLOT(dubReadyReadStandardOutput()));
    connect(m_dubProcess.data(), SIGNAL(readyReadStandardError()), this, SLOT(dubReadyReadStandardError()));
    connect(m_dubProcess.data(), SIGNAL(finished(int)), this, SLOT(dubFinished()));

    m_dubProcess->start(QProcess::ReadOnly);

}

bool DubInitResultsPage::validatePage()
{
    return true;
}

bool DubInitResultsPage::isComplete() const
{
    return m_isCompleted;
}

void DubInitResultsPage::dubReadyReadStandardOutput()
{
    QTextCursor cursor(m_output->document());
    cursor.movePosition(QTextCursor::End);
    QTextCharFormat tf;

    QFont font = m_output->font();
    tf.setFont(font);
    tf.setForeground(m_output->palette().color(QPalette::Text));

    cursor.insertText(QString::fromLocal8Bit(m_dubProcess->readAllStandardOutput()), tf);
}

static QColor mix_colors(const QColor &a, const QColor &b)
{
    return QColor((a.red() + 2 * b.red()) / 3, (a.green() + 2 * b.green()) / 3,
                  (a.blue() + 2* b.blue()) / 3, (a.alpha() + 2 * b.alpha()) / 3);
}

void DubInitResultsPage::dubReadyReadStandardError()
{
    QTextCursor cursor(m_output->document());
    QTextCharFormat tf;

    QFont font = m_output->font();
    QFont boldFont = font;
    boldFont.setBold(true);
    tf.setFont(boldFont);
    tf.setForeground(mix_colors(m_output->palette().color(QPalette::Text), QColor(Qt::red)));

    cursor.insertText(QString::fromLocal8Bit(m_dubProcess->readAllStandardError()), tf);
}

void DubInitResultsPage::dubFinished()
{
    if (m_dubProcess->exitCode() != 0 || m_dubProcess->exitStatus() != QProcess::NormalExit) {
        m_exitCode->setText("Exit code = " + QString::number(m_dubProcess->exitCode())
                            + " (" + m_dubProcess->errorString() + ")");
        m_exitCode->setVisible(true);
        setCompleted(false);
        return;
    }
    setVisible(false);
    setCompleted(true);
}

void DubInitResultsPage::setCompleted(bool b)
{
    m_isCompleted = b;
    emit completeChanged();
}
