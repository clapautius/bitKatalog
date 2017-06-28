#if !defined(XFCAPP_H)
#define XFCAPP_H

// :release:
#define XFCAPP_NAME "bitKatalog"
#define XFCAPP_VERSION "1.1-git"

// :release:
//#define XFC_DEBUG
#define XFC_TIMING // for optimizations

#ifdef XFC_TIMING

#include <chrono>
#include <iostream>

#define MONO_TIME_NOW std::chrono::steady_clock::now().time_since_epoch();
#define MONO_TIME_DIFF_MS(X, Y) std::chrono::duration_cast<std::chrono::milliseconds>(Y - X).count()

#endif

#endif
