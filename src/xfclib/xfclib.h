#if !defined(XFCLIB_H)
#define XFCLIB_H

#include <iostream>
#include <string>

class XfcLogger;

XfcLogger& xfcWarn(XfcLogger&);
XfcLogger& xfcInfo(XfcLogger&);
XfcLogger& xfcDebug(XfcLogger&);
XfcLogger& eol(XfcLogger&);

class XfcLogger
{
public:
    
    XfcLogger(int verboseLevel=0);

    void setVerboseLevel(int);

    int getVerboseLevel() const
    {
        return mVerbLevel;
    }
    
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
