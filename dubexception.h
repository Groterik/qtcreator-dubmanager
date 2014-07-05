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

    virtual void raise() const;
    virtual DubException *clone() const;

    virtual const QString &description() const;

    virtual ~DubException() throw();

private:

    QString errorString;
    
};

} // namespace DubProjectManager

#endif // DUBEXCEPTION_H
