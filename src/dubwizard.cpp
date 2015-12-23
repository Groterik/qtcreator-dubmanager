#include "dubwizard.h"

#include "duboptionspage.h"

#include <projectexplorer/projectexplorerconstants.h>
#include <utils/pathchooser.h>
#include <texteditor/fontsettings.h>
#include <projectexplorer/projectexplorer.h>

#include <QDir>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QFileInfo>
#include <QProcess>
#include <QMessageBox>
#include <QScopedPointer>

const char CATEGORY[] = "I.Projects";
const char DISPLAY_CATEGORY[] = QT_TRANSLATE_NOOP("DubWizard", "Non-Qt Project");
const char WIZARD_ID[] = "WI.DubWizard";
const QString DUB_INIT_NO_TYPE = QLatin1String("<no type>");

using namespace DubProjectManager;

DubWizard::DubWizard()
{
    setWizardKind(ProjectWizard);
    setCategory(CATEGORY);
    setDisplayCategory(DISPLAY_CATEGORY);
    setDescription("Create empty DUB project");
    setDisplayName("DUB Project");
    setId(WIZARD_ID);
    setFlags(PlatformIndependent);
}

#if QTCREATOR_MINOR_VERSION < 5
void DubWizard::runWizard(const QString &path, QWidget *parent,
                          const QString &platform, const QVariantMap &variables)
#else
Utils::Wizard *DubWizard::runWizardImpl(const QString &path, QWidget *parent,
                          const QString &platform, const QVariantMap &variables)
#endif
{
    Q_UNUSED(path)
    Q_UNUSED(platform)
    Q_UNUSED(variables)

    QScopedPointer<DubWizardWidget> widget(new DubWizardWidget);
    if (widget->exec() == DubWizardWidget::Accepted) {
        QDir dir(widget->directory());
        const QString ERROR = tr("Error");
        if (!dir.exists()) {
            QMessageBox::critical(parent, ERROR, tr("Directory %1 does not exist").arg(dir.absolutePath()));
#if QTCREATOR_MINOR_VERSION < 5
            return;
#else
            return widget.take();
#endif
        }
        auto entries = dir.entryList(QStringList() << QLatin1String("dub.sdl") << QLatin1String("dub.json"), QDir::Files);
        if (entries.isEmpty()) {
            QMessageBox::critical(parent, ERROR, tr("File dub.(sdl|json) does not exist in %1").arg(dir.absolutePath()));
#if QTCREATOR_MINOR_VERSION < 5
            return;
#else
            return widget.take();
#endif
        }
        QFileInfo dubFile(dir, entries.front());
        QString errorMessage;
#if QTCREATOR_MINOR_VERSION < 6
        if (!ProjectExplorer::ProjectExplorerPlugin::instance()->openProject(dubFile.absoluteFilePath(), &errorMessage)) {
            QMessageBox::critical(parent, ERROR, tr("Failed to open project: ") + errorMessage);
#else
        auto openProjectResult = ProjectExplorer::ProjectExplorerPlugin::instance()->openProject(dubFile.absoluteFilePath());
        if (!openProjectResult) {
            QMessageBox::critical(parent, ERROR, tr("Failed to open project: ") + openProjectResult.errorMessage());
#endif
#if QTCREATOR_MINOR_VERSION < 5
            return;
#else
            return widget.take();
#endif
        }
    }

#if QTCREATOR_MINOR_VERSION < 5
#else
    return widget.take();
#endif
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

QString DubWizardWidget::directory() const
{
    return m_directory;
}

DubInitSettingsPage::DubInitSettingsPage(DubWizardWidget *parent)
    : m_wizardWidget(parent)
{
    auto layout = new QFormLayout(this);

    QLabel *description = new QLabel;
    description->setWordWrap(true);
    description->setText(tr("Please enter the directory in which you want to build your DUB project. \n"
                          "Qt Creator will execute DUB with the following arguments to initialize the project."));
    description->setSizeIncrement(0, 100);
    layout->addRow(description);

    m_pc = new Utils::PathChooser;
    m_pc->setToolTip("Accepts only absolute paths)");
    layout->addRow("Directory: ", m_pc);
    connect(m_pc, &Utils::PathChooser::pathChanged, [this] {
        this->updateDubCommand();
        emit completeChanged();
    });

    m_dubCommand = new QLabel;
    layout->addRow("DUB command: ", m_dubCommand);

    m_dubInitType = new QComboBox;
    connect(m_dubInitType, &QComboBox::currentTextChanged, [this]() {
        this->updateDubCommand();
    });
    layout->addRow("Init type: ", m_dubInitType);

    m_dubInitExtraArguments = new QLineEdit;
    connect(m_dubInitExtraArguments, &QLineEdit::textChanged, [this]() {
        this->updateDubCommand();
    });
    layout->addRow("Extra arguments: ", m_dubInitExtraArguments);

    setLayout(layout);

    m_dubInitType->addItems(QStringList() << DUB_INIT_NO_TYPE << "minimal" << "vibe.d");
    emit completeChanged();

    setTitle(tr("Configure project"));
}

bool DubInitSettingsPage::validatePage()
{
    const QString path = m_pc->path();
    m_wizardWidget->setDubCommand(getCommand());
    m_wizardWidget->setDubArguments(getArguments());
    m_wizardWidget->setDirectory(path);
    QDir dir(path);
    if (dir.exists()) {
        return QMessageBox::warning(this, tr("Warning"),
                                    tr("Path %1 already exists.\nPress OK to continue creating project or CANCEL to cancel.").arg(dir.absolutePath()),
                                    QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Ok;
    }
    return true;
}

bool DubInitSettingsPage::isComplete() const
{
    const QString path = m_pc->path();
    return !path.isEmpty() && QDir(path).isAbsolute();
}


void DubInitSettingsPage::updateDubCommand()
{
    QChar delim = QLatin1Char(' ');
    m_dubCommand->setText(getCommand() + delim
                          + getArguments().join(delim));
}

QStringList DubInitSettingsPage::getArguments() const
{
    QStringList result;
    result << QLatin1String("init") << m_pc->path();
    auto type = m_dubInitType->currentText();
    if (type != DUB_INIT_NO_TYPE) {
        result << type;
    }
    auto args = m_dubInitExtraArguments->text();
    if (!args.isEmpty()) {
        result << m_dubInitExtraArguments->text();
    }
    return result;
}

QString DubInitSettingsPage::getCommand() const
{
    return DubOptionsPage::executable();
}


DubInitResultsPage::DubInitResultsPage(DubWizardWidget *parent)
    : m_wizardWidget(parent), m_isCompleted(false)
{
    setTitle(tr("Run DUB"));

    auto layout = new QFormLayout;

    m_output = new QPlainTextEdit;
    m_output->setReadOnly(true);
    m_output->setMinimumHeight(15);
    QFont f(TextEditor::FontSettings::defaultFixedFontFamily());
    f.setStyleHint(QFont::TypeWriter);
    m_output->setFont(f);
    QSizePolicy pl = m_output->sizePolicy();
    pl.setVerticalStretch(2);
    m_output->setSizePolicy(pl);
    layout->addRow(m_output);

    m_exitCode = new QLabel;
    layout->addRow("Exit code: ", m_exitCode);
    setLayout(layout);
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

void DubInitResultsPage::cleanupPage()
{
    m_output->clear();
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
    QString exitStatus = QString::number(m_dubProcess->exitCode());
    if (m_dubProcess->exitCode() == 0 && m_dubProcess->exitStatus() == QProcess::NormalExit) {
        setCompleted(true);
    } else {
        exitStatus += " (" + m_dubProcess->errorString() + ")";
        setCompleted(false);
    }

    m_exitCode->setText(exitStatus);
}

void DubInitResultsPage::setCompleted(bool b)
{
    m_isCompleted = b;
    emit completeChanged();
}
