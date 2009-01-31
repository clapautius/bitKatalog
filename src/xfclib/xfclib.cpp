#include "xfclib.h"

static int gXfcVerboseLevel=3;


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


XfcLogger&
XfcLogger::operator<<(XfcLogger& (*pFunc)(XfcLogger&))
{
    return *this;
}


XfcLogger&
XfcLogger::operator<<(string s)
{
    if (mCurrentLevel>=mVerbLevel)
        cout<<s;
    return *this;
}


XfcLogger&
XfcLogger::operator<<(int i)
{
    if (mCurrentLevel>=mVerbLevel)
        cout<<i;
    return *this;
}


XfcLogger gLog(3);
