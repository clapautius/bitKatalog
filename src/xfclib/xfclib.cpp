#include "xfclib.h"

using namespace std;

XfcLogger&
xfcWarn(XfcLogger &rLogger)
{
    rLogger.setLevel(1);
    return rLogger;
}


XfcLogger&
xfcInfo(XfcLogger &rLogger)
{
    rLogger.setLevel(2);
    return rLogger;
}


XfcLogger&
xfcDebug(XfcLogger &rLogger)
{
    rLogger.setLevel(3);
    return rLogger;
}


XfcLogger& eol(XfcLogger &rLogger)
{
    rLogger<<"\n";
    return rLogger;
}


XfcLogger::XfcLogger(int verboseLevel)
{
    mVerbLevel=verboseLevel;
    mCurrentLevel=0;
}


void
XfcLogger::setLevel(int level)
{
    mCurrentLevel=level;
}


void
XfcLogger::setVerboseLevel(int level)
{
    mVerbLevel=level;
}


XfcLogger&
XfcLogger::operator<<(XfcLogger& (*pFunc)(XfcLogger&))
{
    return pFunc(*this);
}


XfcLogger&
XfcLogger::operator<<(std::string s)
{
    if (mVerbLevel>=mCurrentLevel)
        std::cout<<s;
    return *this;
}


XfcLogger&
XfcLogger::operator<<(int i)
{
    if (mVerbLevel>=mCurrentLevel)
        std::cout<<i;
    return *this;
}

XfcLogger gLog;

EntityDiff::EntityDiff()
{
    type=eDiffIdentical;
}
