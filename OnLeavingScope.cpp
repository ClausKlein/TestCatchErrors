// from
// https://crascit.com/2015/06/03/on-leaving-scope-part-2/#more-226
//

#include <functional>
#include <iostream>
#include <string>

namespace {
#if 1
template <typename Func> class OnLeavingScope
{
public:
    // Prevent copying
    OnLeavingScope(const OnLeavingScope&) = delete;
    OnLeavingScope& operator=(const OnLeavingScope&) = delete;

    OnLeavingScope(const Func& f) : m_func(f) {}
    OnLeavingScope(Func&& f) noexcept : m_func(std::move(f)) {}

    ~OnLeavingScope() { m_func(); }

private:
    Func m_func;
};

#else

class OnLeavingScope
{
public:
    // Use std::function so we can support
    // any function-like object
    using Func = std::function<void()>;

    // Prevent copying
    OnLeavingScope(const OnLeavingScope&) = delete;
    OnLeavingScope& operator=(const OnLeavingScope&) = delete;

    OnLeavingScope(const Func& f) : m_func(f) {}
    OnLeavingScope(Func&& f) noexcept : m_func(std::move(f)) {}

    ~OnLeavingScope() { m_func(); }

private:
    Func m_func;
};
#endif

class Printer
{
public:
    Printer(const std::string& msg) : m_msg(msg) { std::cout << "Created" << std::endl; }

    Printer(const Printer& p) : m_msg(p.m_msg) { std::cout << "Copied" << std::endl; }

    Printer(Printer&& p) noexcept : m_msg(std::move(p.m_msg)) { std::cout << "Moved" << std::endl; }

    void operator()() const { std::cout << m_msg << std::endl; }

private:
    std::string m_msg;
};

// Note non-void return type to demonstrate
// we are not restricted to void, unlike
// the std::function approach which is.
bool freeFunc() noexcept
{
    std::cout << "free function" << std::endl;
    return true;
}

void tester()
{
    const auto p = Printer("lvalue");
    auto olsLValue = OnLeavingScope(p);
    std::cout << "Printer lvalue size = " << sizeof(olsLValue) << std::endl;

    std::cout << "----" << std::endl;
    const auto olsRValue = OnLeavingScope(Printer("rvalue"));
    std::cout << "Printer rvalue size = " << sizeof(olsRValue) << std::endl;

    std::cout << "----" << std::endl;
    auto olsLambda = OnLeavingScope([] { std::cout << "lambda" << std::endl; });
    std::cout << "Lambda size = " << sizeof(olsLambda) << std::endl;

    std::cout << "----" << std::endl;
    auto olsFreeFunc = OnLeavingScope(&freeFunc);
    std::cout << "Free function size = " << sizeof(olsFreeFunc) << std::endl;

    std::cout << "----" << std::endl;
}
} // namespace

int main()
{
    tester();

    auto cleanup = OnLeavingScope([] { std::cout << "Good by" << std::endl; });
}
