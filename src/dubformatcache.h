#ifndef DLANGIMAGECACHE_H
#define DLANGIMAGECACHE_H

#include <QMap>
#include <QString>
#include <QStringList>

namespace DubProjectManager {

class DubFormatCache
{
public:

    QString getKeywordDetails(const QString &keyword) const;
    QStringList keywords() const;

    static DubFormatCache &instance();

    static bool isKeywordChar(QChar c);

private:
    DubFormatCache();
    QMap<QString, QString> m_details;
};

} // namespace DlangEditor

#endif // DLANGIMAGECACHE_H
