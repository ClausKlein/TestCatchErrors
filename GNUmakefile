LDFLAGS=-L/usr/local/lib
LDLIBS:=-lboost_filesystem -lboost_system -lpthread

CXX:=clang++
#XXX CXX:=g++-10
CXXFLAGS:=-std=c++2a -Wextra
CPPFLAGS:=-MMD -I/usr/local/include

PROGRAMMS:= filesystem_test OnLeavingScope ScopeGuardTest UncaughtExceptionCounter ScopeGuardOnExit uncaught_exception src/main
#XXX PROGRAMMS+= RangesSamples

SRC:=$(PROGRAMMS:=.cpp)
SRC+= src/ErrorHandler.cpp

OBJ:=$(SRC:.cpp=.o)
DEP:=$(OBJ:.o=.d)

.INTERMEDIATE: $(OBJ)
.PHONY: all test check clean
all: $(PROGRAMMS)

src/main: src/main.o src/ErrorHandler.o
	$(LINK.cc) $< src/ErrorHandler.o -o $@

ScopeGuardTest: LDLIBS:=-lgtest -lgtest_main
ScopeGuardTest: ScopeGuardTest.cpp
	$(LINK.cc) $(LDLIBS) $< -o $@

%: %.o
	$(LINK.cc) $(LDLIBS) $< -o $@

test: ScopeGuardTest
	./$<

check:
	clang-tidy *.hpp

clean:
	rm -rf $(PROGRAMMS) $(OBJ)

-include $(DEP)
