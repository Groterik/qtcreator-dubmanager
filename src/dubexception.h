#ifndef DUBEXCEPTION_H
#define DUBEXCEPTION_H

#include <QException>
#include <QString>

namespace DubProjectManager {

class DubException : public QException
{
public:
    explicit DubException() throw();
    explicit DubException(const QString& descr) throw();

    virtual void raise() const Q_DECL_OVERRIDE;
    virtual DubException *clone() const Q_DECL_OVERRIDE;

    virtual const QString &description() const;
    virtual const char *what() const Q_DECL_NOEXCEPT;

    virtual ~DubException() throw();

private:

    QString errorString;
    mutable std::string str;
    
};

} // namespace DubProjectManager

#endif // DUBEXCEPTION_H
