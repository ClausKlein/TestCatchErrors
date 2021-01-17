#include "ScopeGuard.hpp"

#include <cstdio>
#include <cstdlib>
#include <unistd.h>

int main()
{
    char name[] = "/tmp/deleteme.XXXXXX";
    auto fd = mkstemp(name);
    SCOPE_EXIT
    {
        close(fd);
        unlink(name);
    };

    auto buf = std::malloc(1024 * 1024);
    SCOPE_EXIT { std::free(buf); };
    // ...  use fd and buf ...
}
// NOTE: (if no “;” after lambda, error message is meh)
