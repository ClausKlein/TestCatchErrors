// from
// https://github.com/CppCon/CppCon2015/blob/master/Presentations/Declarative%20Control%20Flow/Declarative%20Control%20Flow%20-%20Andrei%20Alexandrescu%20-%20CppCon%202015.pdf
//

#pragma ones

#include "UncaughtExceptionCounter.hpp"
#include "OnLeavingScope.hpp"

#include <functional>
#include <new>

namespace detail {

template <typename FunctionType> class ScopeGuard
{
    FunctionType function_;

public:
    ScopeGuard() = delete;
    explicit ScopeGuard(const FunctionType& fn) : function_(fn) {}
    explicit ScopeGuard(FunctionType&& fn) : function_(std::move(fn)) {}
    ~ScopeGuard() noexcept
    {
        function_(); // NOTE: Only SCOPE_SUCCESS may throw!
    }

private:
    // Disable copy
    ScopeGuard(const ScopeGuard& /*other*/) = delete;
    ScopeGuard(const ScopeGuard&& /*other*/) = delete;
    // Disable assignment
    ScopeGuard& operator=(ScopeGuard& /*other*/) = delete;
    ScopeGuard& operator=(ScopeGuard&& /*other*/) = delete;
};

enum class ScopeGuardOnExit
{
};

template <typename FunctionType>
ScopeGuard<FunctionType> operator+(ScopeGuardOnExit /*unused*/, FunctionType&& fn)
{
    return ScopeGuard<FunctionType>(std::forward<FunctionType>(fn));
}

template <typename FunctionType, bool executeOnException> class ScopeGuardForNewException
{
    FunctionType function_;
    UncaughtExceptionCounter ec_;

public:
    ScopeGuardForNewException() = delete;
    explicit ScopeGuardForNewException(const FunctionType& fn) : function_(fn) {}
    explicit ScopeGuardForNewException(FunctionType&& fn) : function_(std::move(fn)) {}
    ~ScopeGuardForNewException() noexcept(executeOnException)
    {
        if (executeOnException == ec_.isNewUncaughtException()) {
            function_(); // NOTE: Only SCOPE_SUCCESS may throw!
        }
    }

private:
    // Disable copy
    ScopeGuardForNewException(const ScopeGuardForNewException& /*other*/) = delete;
    ScopeGuardForNewException(const ScopeGuardForNewException&& /*other*/) = delete;
    // Disable assignment
    ScopeGuardForNewException& operator=(ScopeGuardForNewException& /*other*/) = delete;
    ScopeGuardForNewException& operator=(ScopeGuardForNewException&& /*other*/) = delete;
};

enum class ScopeGuardOnFail
{
};
template <typename FunctionType>
ScopeGuardForNewException<typename std::decay<FunctionType>::type, true>
operator+(detail::ScopeGuardOnFail /*unused*/, FunctionType&& fn)
{
    return ScopeGuardForNewException<typename std::decay<FunctionType>::type, true>(
        std::forward<FunctionType>(fn));
}

enum class ScopeGuardOnSuccess
{
};
template <typename FunctionType>
ScopeGuardForNewException<typename std::decay<FunctionType>::type, false>
operator+(detail::ScopeGuardOnSuccess /*unused*/, FunctionType&& fn)
{
    return ScopeGuardForNewException<typename std::decay<FunctionType>::type, false>(
        std::forward<FunctionType>(fn));
}
} // namespace detail

#define CONCATENATE_IMPL(s1, s2) s1##s2
#define CONCATENATE(s1, s2)      CONCATENATE_IMPL(s1, s2)

#ifdef __COUNTER__
#    define ANONYMOUS_VARIABLE(str) CONCATENATE(str, __COUNTER__)
#else
#    define ANONYMOUS_VARIABLE(str) CONCATENATE(str, __LINE__)
#endif

#define SCOPE_SUCCESS                                                                                 \
    auto ANONYMOUS_VARIABLE(SCOPE_ON_SUCCESS) = ::detail::ScopeGuardOnSuccess() + [&]()

#define SCOPE_EXIT                                                                                    \
    auto ANONYMOUS_VARIABLE(SCOPE_EXIT_STATE) = ::detail::ScopeGuardOnExit() + [&]() noexcept

#define SCOPE_FAIL                                                                                    \
    auto ANONYMOUS_VARIABLE(SCOPE_FAIL_STATE) = ::detail::ScopeGuardOnFail() + [&]() noexcept
