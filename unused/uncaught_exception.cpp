#include <cstdio>
#include <exception>

class U
{
public:
    U() = default;
    ~U();
};

class Transaction
{
public:
    Transaction() = default;
    ~Transaction();

private:
    void Rollback() { puts(__func__); }
};

Transaction::~Transaction()
{
    if (std::uncaught_exception()) { // NOTE: unreliable, ONLY if Transaction could be
        Rollback();                  // used from within a dtor (transitively!)
    }
}

void LogStuff()
{
    Transaction t;
    // :::
    // do work
    // :::
}
// oops, if U::~U() is called as part of unwinding another exception
// so uncaught_exception will return true and t will not commit

U::~U()
{
    /* deep call tree that eventually calls LogStuff() */
    LogStuff();
}

// for example:
int main()
{
    try {
        U u;
        throw 1;
    } // U::~U() invoked here
    catch (...) {}
}
