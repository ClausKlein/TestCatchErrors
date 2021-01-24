#include <functional>
#include <iostream>
#include <ranges>
#include <string>
#include <vector>

namespace {
struct User
{
    std::string name;
    int age;
};
const std::vector<User> users = {
    {"Susi", 17}, {"Hans", 33}, {"Toni", 15}, {"Sissi", 18}, {"Baby", 1}, {"Erwin", 71},
};
bool underage(const User& user) { return user.age < 18; };

void foo()
{
    for (const auto& user : users | std::views::filter(std::not_fn(underage)) |
                         std::views::transform([](const auto& user) { return user; })) {
        std::cout << user.name << ":\t" << user.age << '\n';
    }
    std::cout << std::endl;
}

void bar()
{
    // XXX std::vector<int> ints{0, 1, 2, 3, 4, 5};
    auto even = [](int i) { return 0 == i % 2; };
    auto square = [](int i) { return i * i; };

    for (int i :
         std::ranges::iota_view{0, 5} | std::views::filter(even) | std::views::transform(square)) {
        std::cout << i << ' ';
    }
    std::cout << std::endl;
}
} // namespace

int main()
{
    foo();
    bar();
}
