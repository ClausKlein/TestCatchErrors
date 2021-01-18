// from
// https://github.com/CppCon/CppCon2015/blob/master/Presentations/Declarative%20Control%20Flow/Declarative%20Control%20Flow%20-%20Andrei%20Alexandrescu%20-%20CppCon%202015.pdf
//

#pragma ones

#include "UncaughtExceptionCounter.hpp"

#include <functional>
#include <new>
#include <type_traits>
#include <utility>

namespace detail {

class ScopeGuardImplBase {
 public:
  void dismiss() noexcept { dismissed_ = true; }

 protected:
  ScopeGuardImplBase() noexcept : dismissed_(false) {}

  static void warnAboutToCrash() noexcept;
  static ScopeGuardImplBase makeEmptyScopeGuard() noexcept {
    return ScopeGuardImplBase{};
  }

  template <typename T>
  static const T& asConst(const T& t) noexcept {
    return t;
  }

  bool dismissed_;
};

template <typename FunctionType, bool InvokeNoexcept>
class ScopeGuardImpl : public ScopeGuardImplBase {
 public:
  explicit ScopeGuardImpl(FunctionType& fn) noexcept(
      std::is_nothrow_copy_constructible<FunctionType>::value)
      : ScopeGuardImpl(
            asConst(fn),
            makeFailsafe(
                std::is_nothrow_copy_constructible<FunctionType>{},
                &fn)) {}

  explicit ScopeGuardImpl(const FunctionType& fn) noexcept(
      std::is_nothrow_copy_constructible<FunctionType>::value)
      : ScopeGuardImpl(
            fn,
            makeFailsafe(
                std::is_nothrow_copy_constructible<FunctionType>{},
                &fn)) {}

  explicit ScopeGuardImpl(FunctionType&& fn) noexcept(
      std::is_nothrow_move_constructible<FunctionType>::value)
      : ScopeGuardImpl(
            std::move_if_noexcept(fn),
            makeFailsafe(
                std::is_nothrow_move_constructible<FunctionType>{},
                &fn)) {}

  ScopeGuardImpl(ScopeGuardImpl&& other) noexcept(
      std::is_nothrow_move_constructible<FunctionType>::value)
      : function_(std::move_if_noexcept(other.function_)) {
    // If the above line attempts a copy and the copy throws, other is
    // left owning the cleanup action and will execute it (or not) depending
    // on the value of other.dismissed_. The following lines only execute
    // if the move/copy succeeded, in which case *this assumes ownership of
    // the cleanup action and dismisses other.
    dismissed_ = std::exchange(other.dismissed_, true);
  }

  ~ScopeGuardImpl() noexcept(InvokeNoexcept) {
    if (!dismissed_) {
      execute();
    }
  }

 private:
  static ScopeGuardImplBase makeFailsafe(std::true_type, const void*) noexcept {
    return makeEmptyScopeGuard();
  }

  template <typename Fn>
  static auto makeFailsafe(std::false_type, Fn* fn) noexcept
      -> ScopeGuardImpl<decltype(std::ref(*fn)), InvokeNoexcept> {
    return ScopeGuardImpl<decltype(std::ref(*fn)), InvokeNoexcept>{
        std::ref(*fn)};
  }

  template <typename Fn>
  explicit ScopeGuardImpl(Fn&& fn, ScopeGuardImplBase&& failsafe)
      : ScopeGuardImplBase{}, function_(std::forward<Fn>(fn)) {
    failsafe.dismiss();
  }

  void* operator new(std::size_t) = delete;

  void execute() noexcept {
    try {
      function_();
    } catch(...) {}
  }

  FunctionType function_;
};

template <typename F, bool INE>
using ScopeGuardImplDecay = ScopeGuardImpl<typename std::decay<F>::type, INE>;

} // namespace detail

/**
 * ScopeGuard is a general implementation of the "Initialization is
 * Resource Acquisition" idiom.  Basically, it guarantees that a function
 * is executed upon leaving the current scope unless otherwise told.
 *
 * The makeGuard() function is used to create a new ScopeGuard object.
 * It can be instantiated with a lambda function, a std::function<void()>,
 * a functor, or a void(*)() function pointer.
 *
 *
 * Usage example: Add a friend to memory if and only if it is also added
 * to the db.
 *
 * void User::addFriend(User& newFriend) {
 *   // add the friend to memory
 *   friends_.push_back(&newFriend);
 *
 *   // If the db insertion that follows fails, we should
 *   // remove it from memory.
 *   auto guard = makeGuard([&] { friends_.pop_back(); });
 *
 *   // this will throw an exception upon error, which
 *   // makes the ScopeGuard execute UserCont::pop_back()
 *   // once the Guard's destructor is called.
 *   db_->addFriend(GetName(), newFriend.GetName());
 *
 *   // an exception was not thrown, so don't execute
 *   // the Guard.
 *   guard.dismiss();
 * }
 *
 * Examine ScopeGuardTest.cpp for some more sample usage.
 *
 * Stolen from:
 *   Andrei's and Petru Marginean's CUJ article:
 *     http://drdobbs.com/184403758
 *   and the loki library:
 *     http://loki-lib.sourceforge.net/index.php?n=Idioms.ScopeGuardPointer
 *   and triendl.kj article:
 *     http://www.codeproject.com/KB/cpp/scope_guard.aspx
 */
template <typename F>
[[nodiscard]] detail::ScopeGuardImplDecay<F, true> makeGuard(F&& f) noexcept {
  return detail::ScopeGuardImplDecay<F, true>(static_cast<F&&>(f));
}


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
