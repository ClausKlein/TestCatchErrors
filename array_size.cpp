#include <array>
#include <iostream>

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
} // namespace

int main()
{
    foo();
    bar();
}

/***
 * C++ array size: 11
 *   C array size: 11
 */
