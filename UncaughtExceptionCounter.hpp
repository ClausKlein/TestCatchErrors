#pragma ones

#include <exception>

class UncaughtExceptionCounter
{
    int getUncaughtExceptionCount() noexcept;
    int exceptionCount_;

public:
    UncaughtExceptionCounter() : exceptionCount_(std::uncaught_exceptions()) {}
    bool newUncaughtException() noexcept { return std::uncaught_exceptions() > exceptionCount_; }
};
