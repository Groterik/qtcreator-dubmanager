#include "dubproject.h"

#include "dubprojectmanagerconstants.h"

#include "dubmanager.h"
#include "dubfile.h"
#include "dubprojectnode.h"

#include "dubexception.h"

#include <projectexplorer/projectexplorerconstants.h>
#include <coreplugin/icontext.h>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFileSystemWatcher>

DubProject::DubProject(DubManager *manager, const QString &filename)
    : m_manager(manager),
      m_filename(filename),
      m_rootNode(new DubProjectNode(filename))
{
    setId(DubProjectManager::Constants::DUBPROJECT_ID);
    setProjectContext(Core::Context(DubProjectManager::Constants::PROJECTCONTEXT));
    setProjectLanguages(Core::Context(ProjectExplorer::Constants::LANG_CXX));

    m_projectName = QFileInfo(filename).absoluteDir().dirName();

    m_file = new DubFile(filename, this);
    m_watcher = new QFileSystemWatcher(this);

    connect(m_watcher, SIGNAL(fileChanged(QString)), this, SLOT(dubFileChanged(QString)));

    parseConfig();
}

DubProject::~DubProject()
{
}

QString DubProject::displayName() const
{
    return m_projectName;
}

Core::IDocument *DubProject::document() const
{
    return m_file;
}

ProjectExplorer::IProjectManager *DubProject::projectManager() const
{
    return m_manager;
}

ProjectExplorer::ProjectNode *DubProject::rootProjectNode() const
{
    return m_rootNode;
}

QStringList DubProject::files(ProjectExplorer::Project::FilesMode fileMode) const
{
    Q_UNUSED(fileMode);
    return m_files;
}

void DubProject::parseConfig()
{
    QFile file(m_filename);
    if (!file.open(QIODevice::ReadOnly)) return;
    QByteArray blob = file.readAll();
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(blob, &err);

    if (err.error != QJsonParseError::NoError) {
        throw DubException(tr("Failed to parse config: ") + err.errorString());
    }

    QJsonObject root = doc.object();
    QJsonValue name = root.value(QString::fromUtf8("name"));
    if (name.isNull() || name.isUndefined() || !name.isString()) {
        throw DubException(tr("Failed to parse project name in config"));
    }
    m_projectName = name.toString();

    QJsonValue sourcePaths = root.value(QString::fromUtf8("sourcePaths"));
    QList<QString> directories;
    if (sourcePaths.isNull() || sourcePaths.isUndefined()) {
        directories.push_back(QString::fromUtf8("source"));
        directories.push_back(QString::fromUtf8("src"));
    } else if (!sourcePaths.isArray()) {
        throw DubException(tr("Failed to parse sourcePaths in ") + m_filename);
    } else {
        QJsonArray paths = sourcePaths.toArray();
        foreach (QJsonValue p, paths) {
            if (!p.isString()) {
                throw DubException(tr("Failed to parse path in sourcePaths in ") + m_filename);
            }
            directories.push_back(p.toString());
        }
    }

    m_files = scanDirectories(directories, m_filename);
    m_rootNode->setDisplayName(m_projectName);

    // build tree
    m_rootNode->clear();
    foreach (const QString& filename, m_files) {
        m_rootNode->addFilePath(filename);
    }
    m_rootNode->addFilePath(m_filename);
    mergeProjectNode(m_rootNode);

}

const QString DubProject::buildDirectory() const
{
    return m_buildDirectory;
}

QStringList DubProject::scanDirectories(QStringList directories, const QString& root)
{
    QStringList result;
    foreach (const QString& dirname, directories) {
        QString absoluteDirname = QFileInfo(root).absoluteDir().absoluteFilePath(dirname);
        QDirIterator iterator(absoluteDirname, QDirIterator::Subdirectories);
        while (iterator.hasNext()) {
            iterator.next();
            if (!iterator.fileInfo().isDir()) {
                QString filename = iterator.filePath();
                if (filename.endsWith(".d") || filename.endsWith(".di")) {
                    result.push_back(filename);
                }
            }
        }
    }
    return result;
}

void DubProject::dubFileChanged(const QString &filename)
{
    Q_UNUSED(filename);
    parseConfig();
}
