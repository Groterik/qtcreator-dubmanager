#include "dubproject.h"

#include "dubmanager.h"
#include "dubfile.h"
#include "dubprojectnode.h"

#include "dubexception.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

DubProject::DubProject(DubManager *manager, const QString &filename)
    : m_manager(manager), m_filename(filename)
{
}

DubProject::~DubProject()
{
}

QString DubProject::displayName() const
{
    return m_projectName;
}

Core::Id DubProject::id() const
{
    return Core::Id(m_projectName);
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
    if (name.isNull() || !name.isString()) {
        throw DubException(tr("Failed to parse project name in config"));
    }
    m_projectName = name.toString();

    QJsonValue sourcePaths = root.value(QString::fromUtf8("sourcePaths"));
    QList<QString> directories;
    if (sourcePaths.isNull()) {
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
}
