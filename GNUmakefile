LDFLAGS=-L/usr/local/lib
LDLIBS:=-lboost_filesystem -lboost_system -lpthread

CXX:=clang++
#!NO CXX:=g++-10
CXX?=g++
CXXFLAGS:=-std=c++2a -Wextra -DBOOST_FILESYSTEM_NO_DEPRECATED -UCXX_FILESYSTEM_HAVE_FS
CPPFLAGS:=-MMD -I/usr/local/include

PROGRAMMS:= FilesystemTest OnLeavingScope UncaughtExceptionCounter ScopeGuardOnExit
#NO! PROGRAMMS+= ScopeGuardTest src/main uncaught_exception
PROGRAMMS+= RangesSamples

SRC:=$(PROGRAMMS:=.cpp)
SRC+= src/ErrorHandler.cpp

OBJ:=$(SRC:.cpp=.o)
DEP:=$(OBJ:.o=.d)

.INTERMEDIATE: $(OBJ)
.PHONY: all build test check clean distclean
all: build $(PROGRAMMS)

build:
	cmake -G Ninja -B $@ -S .
	ninja -C $@

src/main: src/main.o src/ErrorHandler.o
	$(LINK.cc) $< $(LDLIBS) src/ErrorHandler.o -o $@

ScopeGuardTest: LDLIBS:=-lgtest -lgtest_main
ScopeGuardTest: ScopeGuardTest.cpp
	$(LINK.cc) $< $(LDLIBS) -o $@

%: %.o
	$(LINK.cc) $< $(LDLIBS) -o $@

test: build
	ninja -C $< $@

check:
	clang-tidy *.hpp

clean:
	rm -rf $(PROGRAMMS) $(OBJ)

distclean: clean
	rm -rf build $(DEP)

-include $(DEP)
