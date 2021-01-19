LDFLAGS=-L/usr/local/lib
LDLIBS:=-lboost_filesystem
CXXFLAGS:=-std=c++2a -Wextra
CPPFLAGS:=-MMD

PROGRAMMS:= ScopeGuardTest UncaughtExceptionCounter ScopeGuardOnExit uncaught_exception src/main

SRC:=$(PROGRAMMS:=.cpp)
SRC+= src/ErrorHandler.cpp

OBJ:=$(SRC:.cpp=.o)
DEP:=$(OBJ:.o=.d)

.INTERMEDIATE: $(OBJ)
.PHONY: all check clean
all: $(PROGRAMMS)

src/main: src/main.o src/ErrorHandler.o
	$(LINK.cc) $< src/ErrorHandler.o -o $@

ScopeGuardTest: LDLIBS:=-lfolly -lgtest -lglog -lgtest_main
ScopeGuardTest: ScopeGuardTest.cpp
	$(LINK.cc) $(LDLIBS) $< -o $@


%: %.cpp
	$(LINK.cc) $(LDLIBS) $< -o $@

check:
	clang-tidy *.hpp

clean:
	rm -rf $(PROGRAMMS) $(OBJ)

-include $(DEP)
