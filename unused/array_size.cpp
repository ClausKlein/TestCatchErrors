#include <array>
#include <iostream>
#include <string>

namespace {
constexpr std::size_t len = 11;

void foo()
{
    std::array<int, len> a1;
    int a2[len];
    std::cout << "C++ array size: " << std::size(a1) << '\n'
              << "  C array size: " << std::size(a2) << '\n';
}

template <class T, size_t N> void size(T (&)[N]) { std::cout << "  C array size: " << N << '\n'; }

template <class T, size_t N> void size(const std::array<T, N>&)
{
    std::cout << "C++ array size: " << N << '\n';
}

void bar()
{
    std::array<int, len> a1;
    int a2[len];
    size(a1);
    size(a2);
}

void oops(const std::string_view& value)
{
    std::cout << value << " std::size(string_view): " << std::size(value) << std::endl;
}

} // namespace

int main()
{
    foo();
    bar();

    constexpr uint32_t value{42};
    std::cout << value << " sizeof(unit32_t): " << sizeof(value) << std::endl;

    {
        constexpr std::string_view hello("holla!");
        oops(hello);
    }
    {
        const std::string hello("holla!");
        oops(hello);
    }
    {
        const char* hello("holla!");
        oops(hello);
    }
}

/***
 * C++ array size: 11
 *   C array size: 11
 */
