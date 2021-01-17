#pragma ones

#include <exception>

class UncaughtExceptionCounter
{
    int exceptionCount_;

public:
    UncaughtExceptionCounter() : exceptionCount_(std::uncaught_exceptions()) {}
    bool isNewUncaughtException() const noexcept { return std::uncaught_exceptions() > exceptionCount_; }
};
