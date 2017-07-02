#if !defined(XFCAPP_H)
#define XFCAPP_H

// :release:
#define XFCAPP_NAME "bitKatalog"
#define XFCAPP_VERSION "1.1-git"

// :release:
//#define XFC_DEBUG
#define XFC_TIMING  // for optimizations

#ifdef XFC_TIMING

#include <chrono>
#include <iostream>

#define MONO_TIME_NOW std::chrono::steady_clock::now().time_since_epoch();
#define MONO_TIME_DIFF_MS(X, Y) \
    std::chrono::duration_cast<std::chrono::milliseconds>(Y - X).count()

#define XFC_TIMING_FN_ENTRY auto xfc_entry_fn_moment = MONO_TIME_NOW

#define XFC_TIMING_FN_EXIT                                                  \
    auto xfc_exit_fn_moment = MONO_TIME_NOW;                                \
    std::cout << __FUNCTION__ << ": time (ms): "                            \
              << MONO_TIME_DIFF_MS(xfc_entry_fn_moment, xfc_exit_fn_moment) \
              << std::endl;

#endif

#endif
