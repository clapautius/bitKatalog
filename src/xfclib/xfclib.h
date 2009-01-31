#if !defined(XFCLIB_H)
#define XFCLIB_H

#include <iostream>
#include <string>

using namespace std;

class XfcLogger;

XfcLogger& xfcWarn(XfcLogger&);
XfcLogger& xfcInfo(XfcLogger&);
XfcLogger& xfcDebug(XfcLogger&);
XfcLogger& eol(XfcLogger&);

class XfcLogger
{
public:
    
    XfcLogger(int verboseLevel);

    void setLevel(int);

    XfcLogger& operator<<(XfcLogger& (*)(XfcLogger&));
    XfcLogger& operator<<(std::string);
    XfcLogger& operator<<(int);

private:

    int mVerbLevel;

    int mCurrentLevel;
};

extern XfcLogger gLog;

#endif
