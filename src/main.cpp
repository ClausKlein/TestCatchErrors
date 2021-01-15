#include "ErrorHandler.h"

#include <exception>
#include <iostream>
#include <thread>
#include <string>

namespace {
struct MenuInfo
{
    const char* text;
    void (*call)();
};

void s_thread(void (*fkt)())
{
    std::thread t{*fkt};
    t.join();
}

void s_funcOk() {}
void s_threadOk() { s_thread(&s_funcOk); }

void s_funcExit() { exit(0); }
void s_threadExit() { s_thread(&s_funcExit); }

void s_funcAbort() { abort(); }
void s_threadAbort() { s_thread(&s_funcAbort); }

void s_funcSuspend()
{
    char* ptr = nullptr;
    char z = ptr[rand()];
    std::cout << "This output should never apear: " << z << std::endl;
}
void s_threadSuspend() { s_thread(&s_funcSuspend); }

void s_funcException() { throw new std::runtime_error("Not implemented yet!"); }
void s_threadException() { s_thread(&s_funcException); }

const MenuInfo s_calls[] = {{"Function calls ok", &s_funcOk},
                          {"Thread   calls ok", &s_threadOk},
                          {"Function calls exit", &s_funcExit},
                          {"Thread   calls exit", &s_threadExit},
                          {"Function calls abort", &s_funcAbort},
                          {"Thread   calls abort", &s_threadAbort},
                          {"Function suspend", &s_funcSuspend},
                          {"Thread   suspend", &s_threadSuspend},
                          {"Function call uncatched expception", &s_funcException},
                          {"Thread   call uncatched expception", &s_threadException},
                          {}
};

void s_usage(const char* name)
{
    std::cout << "Call " << name
              << " <sim error> ...\n"
                 "========================================\n"
                 "Available functions:\n"
                 "----------------------------------------\n";
    for (auto i = 0u; s_calls[i].text != nullptr; ++i) {
        std::cout << '\t' << i << ". " << s_calls[i].text << std::endl;
    }

    exit(EXIT_SUCCESS);
}
} // namespace

int main(int argc, char* argv[])
{
    if (argc == 1) {
        s_usage(*argv);
    }

    // ================
    ErrHdlr_register();

    std::cout << "Try to call given handler:\n";
    for (auto i = 1; i < argc; ++i) {
        auto idx = std::stoul(std::string(argv[i]));
        {
            std::cout << "Calling " << s_calls[idx].text << std::endl;
            (s_calls[idx].call)();
        }
    }

    ErrHdlr_cleanup();
    // ================
}
