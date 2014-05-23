#ifndef DUBEXCEPTION_H
#define DUBEXCEPTION_H

#include <QException>
#include <QString>

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

#endif // DUBEXCEPTION_H
