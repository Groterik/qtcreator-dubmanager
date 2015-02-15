#include "dubexception.h"

using namespace DubProjectManager;

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

const char *DubException::what() const Q_DECL_NOEXCEPT
{
    str = errorString.toStdString();
    return str.c_str();
}

DubException::~DubException() throw()
{
}
