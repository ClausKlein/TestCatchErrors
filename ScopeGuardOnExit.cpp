#include "ScopeGuard.hpp"

#include <boost/filesystem.hpp>

#include <array>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <unistd.h>

namespace bf = boost::filesystem;

namespace {
void copy_file_transact(const bf::path& from, const bf::path& to)
{
    if (from != to) {
        bf::path t = to.native() + ".deleteme";
        SCOPE_FAIL { ::remove(t.c_str()); };
        bf::copy_file(from, t); // NOTE: may throw! CK
        bf::rename(t, to);
    }
}

void move_file_transact(const bf::path& from, const bf::path& to)
{
    if (from != to) {
        copy_file_transact(from, to); // NOTE: may throw! CK
        SCOPE_FAIL { ::remove(to.c_str()); };
        bf::remove(from);
    }
}

void process(char* const buf, size_t len)
{
    if (!len) {
        return;
    }

    const auto save = buf[len - 1];
    buf[len - 1] = static_cast<char>(255);
    SCOPE_EXIT { buf[len - 1] = save; };
    for (auto p = buf; p < buf + len; p++) {
        switch (auto c = *p) {
            //...
        }
    }
}

void foo()
{
    constexpr size_t bufferlen{1024 * 1024};
    char* buf = static_cast<char*>(std::malloc(bufferlen));
    SCOPE_EXIT
    {
        puts("free(buf)"); // TRACE
        std::free(buf);
    };
    process(buf, bufferlen);
}

int string2int(const std::string& s)
{
    int r = std::stoi(s); // NOTE: may throw! CK
    SCOPE_SUCCESS
    {
        puts(std::to_string(r).c_str());    // TRACE
        assert(std::to_string(r) == s);
    };
    return r;
}
} // namespace

int main(int argc, char** argv)
{
    foo();

    std::array<char, 21> name{"/tmp/deleteme.XXXXXX"};
    auto fd = mkstemp(name.data());
    SCOPE_EXIT
    {
        close(fd);
        puts(name.data());  // TRACE
        unlink(name.data());
    };

    try {
        bf::path from(*argv);
        bf::path tmp("/tmp/deleteme");
        SCOPE_EXIT { ::remove(tmp.c_str()); };
        SCOPE_SUCCESS { throw std::runtime_error("This shall not happen!"); };

        copy_file_transact(from, name.data());
        move_file_transact(from, from);
        move_file_transact(from, tmp);
        move_file_transact(tmp, "/usr/share/doc/noaccess");
    } catch (std::exception& e) {
        puts(e.what()); // ERROR
    }

    if (argc > 1) {
        ++argv;
    }
    return string2int(*argv);
}
// NOTE: if no “;” after lambda, error message is inevitable!
