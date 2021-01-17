#pragma ones

#include <utility>

// see https://www.drdobbs.com/article/print?articleId=184403758&siteSectionName=cpp
// and
// https://github.com/CppCon/CppCon2015/blob/master/Presentations/Declarative%20Control%20Flow/Declarative%20Control%20Flow%20-%20Andrei%20Alexandrescu%20-%20CppCon%202015.pdf

namespace detail {

class ScopeGuardImplBase
{
public:
    void Dismiss() const { dismissed_ = true; }

protected:
    ScopeGuardImplBase() : dismissed_(false) {}
    ScopeGuardImplBase(const ScopeGuardImplBase& other) : dismissed_(other.dismissed_)
    {
        other.Dismiss();
    }
    ~ScopeGuardImplBase() {} // nonvirtual (see below why)
    mutable bool dismissed_;

private:
    // Disable assignment
    ScopeGuardImplBase& operator=(const ScopeGuardImplBase&);
};

template <typename Fun> class ScopeGuard : public ScopeGuardImplBase
{
public:
    ScopeGuard(const Fun& fun) : fun_(fun) {}
    ~ScopeGuard()
    {
        if (!dismissed_) {
            try {
                (fun_)();
            } catch (...) {} // NOTE: In the realm of exceptions, it is fundamental that
            // you can do nothing if your "undo/recover" action fails.
            // You attempt an undo operation, and you move on regardless
            // whether the undo operation succeeds or not.
        }
    }

private:
    Fun fun_;
};

enum class ScopeGuardOnExit
{
};

template <typename Fun> ScopeGuard<Fun> operator+(ScopeGuardOnExit, Fun&& fn)
{
    return ScopeGuard<Fun>(std::forward<Fun>(fn));
}
} // namespace detail

#define CONCATENATE_IMPL(s1, s2) s1##s2
#define CONCATENATE(s1, s2)      CONCATENATE_IMPL(s1, s2)

#ifdef __COUNTER__
#    define ANONYMOUS_VARIABLE(str) CONCATENATE(str, __COUNTER__)
#else
#    define ANONYMOUS_VARIABLE(str) CONCATENATE(str, __LINE__)
#endif

#define SCOPE_EXIT auto ANONYMOUS_VARIABLE(SCOPE_EXIT_STATE) = ::detail::ScopeGuardOnExit() + [&]()
