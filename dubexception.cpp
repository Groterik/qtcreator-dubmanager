#include "dubexception.h"

DubException::DubException() throw() :
    QException()
{
}

DubException::DubException(const QString &descr) throw() :
    errorString(descr)
{
}

void DubException::raise() const
{
    throw *this;
}


DubException *DubException::clone() const
{
    return new DubException(*this);
}

const QString &DubException::description() const
{
    return errorString;
}

DubException::~DubException() throw()
{
}
