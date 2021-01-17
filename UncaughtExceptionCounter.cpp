#include "UncaughtExceptionCounter.hpp"

#include <iostream>
#include <stdexcept>

class RAII
{
    UncaughtExceptionCounter ec_;

public:
    RAII() = default;
    ~RAII()
    {
        std::cout << (!ec_.newUncaughtException() ? "~RAII() called normally\n"
                                                  : "~RAII() called during stack unwinding\n");
    }
};

// NOTE: first creates a guard object and records the number of uncaught
// exceptions in its constructor. The output is performed by the guard
// object's destructor unless foo() throws (in which case the number of
// uncaught exceptions in the destructor is greater than what the
// constructor observed)
int main()
{
    RAII f1;
    try {
        RAII f2;
        std::cout << "Exception thrown\n";
        throw std::runtime_error("test exception");
    } catch (const std::exception& e) {
        std::cout << "Exception caught: " << e.what() << '\n';
    }
}
