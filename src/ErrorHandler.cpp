#include "ErrorHandler.h"

#include <csignal>
#include <execinfo.h>
#include <iostream>

// To generate a C-Library which also support terminate this dirty workaround
// at least for gcc can help
//
// #pragma weak _ZSt13set_terminatePFvvE
// extern void (*)(void) _ZSt13set_terminatePFvvE( void);

namespace {
bool s_doOutput = true;

void s_dumpCallstack(const std::string& msg)
{
    if (s_doOutput) {
        constexpr size_t cnt{128};
        static void* callstack[cnt];

        int frames = backtrace(callstack, cnt);
        char** strs = backtrace_symbols(callstack, frames);
        std::cerr << msg << ": dump call-stack (" << frames << " entries)\n"
                  << "============================================================\n";
        for (auto i = 0; i < frames; ++i) {
            std::cerr << strs[i] << std::endl;
        }
        free(strs);
        std::cerr << "Stack backtrace finished!\n";
    }
}

void s_callExit() { s_dumpCallstack(__func__); }

void s_callAbort(int)
{
    s_dumpCallstack(__func__);
    //XXX ::quick_exit(EXIT_FAILURE);
    ErrHdlr_cleanup();
}

void s_callIllegal(int)
{
    s_dumpCallstack(__func__);
    //XXX ::quick_exit(EXIT_FAILURE);
    ErrHdlr_cleanup();
}

void s_callSegmentationFault(int)
{
    s_dumpCallstack(__func__);
    //XXX ::quick_exit(EXIT_FAILURE);
    ErrHdlr_cleanup();
}

void s_callTerminate()
{
    s_dumpCallstack(__func__);
    //XXX ::quick_exit(EXIT_FAILURE);
    ErrHdlr_cleanup();
}
} // namespace

void ErrHdlr_register()
{
    ::atexit(s_callExit);
    //TODO ::at_quick_exit(s_callExit);
    std::signal(SIGABRT, s_callAbort);
    std::signal(SIGILL, s_callIllegal);
    std::signal(SIGSEGV, s_callSegmentationFault);
    std::set_terminate(s_callTerminate);
}

void ErrHdlr_cleanup() { s_doOutput = false; exit(EXIT_SUCCESS); }
