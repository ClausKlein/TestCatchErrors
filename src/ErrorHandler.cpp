#include "ErrorHandler.hpp"

#include <csignal>
#include <execinfo.h>
#include <filesystem>
#include <fstream>
#include <iostream>

// TODO: #define NO_BOOST
#ifndef NO_BOOST
#    ifndef _GNU_SOURCE
#        define _GNU_SOURCE
#    endif
#    include <boost/stacktrace.hpp>
#endif

// see too:
// https://www.boost.org/doc/libs/1_75_0/doc/html/stacktrace/getting_started.html#stacktrace.getting_started.handle_terminates_aborts_and_seg
// https://en.cppreference.com/w/cpp/error/set_terminate
// https://man7.org/linux/man-pages/man3/backtrace.3.html
// and
// https://stackoverflow.com/questions/9758495/what-is-the-difference-between-stdquick-exit-and-stdabort-and-why-was-stdq
//
// To generate a C-Library which also support terminate this dirty workaround
// at least for gcc can help
//
// #pragma weak _ZSt13set_terminatePFvvE
// extern void (*)(void) _ZSt13set_terminatePFvvE( void);

namespace {
bool s_doOutput = true;

#ifndef NO_BOOST
void s_dumpCallstack(const std::string& /*msg*/)
{
    if (s_doOutput) {
        boost::stacktrace::safe_dump_to("./backtrace.dump");
    }
}

#else
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
        std::cerr << "Stack backtrace finished!" << std::endl;
    }
}
#endif

void s_callExit()
{
    s_dumpCallstack(__func__);
    // XXX ::quick_exit(EXIT_FAILURE);
    _Exit(EXIT_SUCCESS);
}

void s_callAbort(int signum)
{
    ::signal(signum, SIG_DFL);
    s_dumpCallstack(__func__);
    // XXX ::quick_exit(EXIT_FAILURE);
    _Exit(EXIT_SUCCESS);
}

void s_callBus(int signum)
{
    ::signal(signum, SIG_DFL);
    s_dumpCallstack(__func__);
    // XXX ::quick_exit(EXIT_FAILURE);
    _Exit(EXIT_SUCCESS);
}

void s_callIllegal(int signum)
{
    ::signal(signum, SIG_DFL);
    s_dumpCallstack(__func__);
    // XXX ::quick_exit(EXIT_FAILURE);
    _Exit(EXIT_SUCCESS);
}

void s_callSegmentationFault(int signum)
{
    ::signal(signum, SIG_DFL);
    s_dumpCallstack(__func__);
    // XXX ::quick_exit(EXIT_FAILURE);
    _Exit(EXIT_SUCCESS);
}

void s_callTerminate()
{
    s_dumpCallstack(__func__);
    // XXX ::quick_exit(EXIT_FAILURE);
    _Exit(EXIT_SUCCESS);
}
} // namespace

void ErrHdlr_register()
{
#ifndef NO_BOOST
    if (std::filesystem::exists("./backtrace.dump")) {
        // there is a backtrace
        std::ifstream ifs("./backtrace.dump");

        boost::stacktrace::stacktrace st = boost::stacktrace::stacktrace::from_dump(ifs);
        std::cerr << "Previous run crashed:\n" << st << std::endl;

        // cleaning up
        ifs.close();
        std::filesystem::remove("./backtrace.dump");
    }
#endif

    ::atexit(s_callExit);
    ::signal(SIGABRT, s_callAbort);
    ::signal(SIGBUS, s_callBus);
    ::signal(SIGILL, s_callIllegal);
    ::signal(SIGSEGV, s_callSegmentationFault);
    // TODO ::at_quick_exit(s_callExit);
    // NOTE: C++ only! CK
    std::set_terminate(s_callTerminate);
}

void ErrHdlr_cleanup() { s_doOutput = false; }
