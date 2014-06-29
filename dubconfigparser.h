#ifndef DUBCONFIGPARSER_H
#define DUBCONFIGPARSER_H

#include <QObject>
#include <QStringList>
#include <QMap>

class ConfigurationInfo
{
public:
    const QStringList &files() const;
    const QStringList &directories() const;
    const QString &path() const;
    const QStringList &importPaths() const;
    const QString &targetName() const;
    const QString &workingDirectory() const;
    const QString &targetFilename() const;
    const QString &targetType() const;
    const QString &targetPath() const;
private:
    QStringList m_files;
    QStringList m_directories;
    QString m_path;
    QStringList m_importPaths;
    QString m_targetName;
    QString m_workingDirectory;
    QString m_targetFilename;
    QString m_targetType;
    QString m_targetPath;

    friend class DubConfigParser;
};

class DubConfigParser : public QObject
{
    Q_OBJECT
public:
    explicit DubConfigParser(const QString& directory, QObject *parent = 0);

    bool parse();

    const ConfigurationInfo& getState(const QString& conf) const;

    const QStringList &configurationsList() const;
    const QStringList &buildTypesList() const;

    const ConfigurationInfo& configurationInfo(const QString& conf) const;

signals:

public slots:

private:

    QStringList readList(const QStringList &args);

    QString m_directory;
    QString m_errorString;
    QStringList m_configurations;
    QStringList m_buildTypes;

    typedef QMap<QString, ConfigurationInfo> StateMap;
    StateMap m_states;

    bool parseDescribe(QByteArray array, ConfigurationInfo &state);
};

#endif // DUBCONFIGPARSER_H
