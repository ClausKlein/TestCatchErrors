LDFLAGS=-L/usr/local/lib
LDLIBS:=-lboost_filesystem -lboost_system -lpthread

# CXX:=clang++
#!NO CXX:=g++-10
CXX?=g++
CXXFLAGS:=-std=c++17 -Wextra -DBOOST_FILESYSTEM_NO_DEPRECATED -UCXX_FILESYSTEM_HAVE_FS
CPPFLAGS:=-MMD -I/usr/local/include

CMAKE_STAGING_PREFIX?=$(shell realpath $(CURDIR)/../stagedir)

PROGRAMMS:= FilesystemTest OnLeavingScope UncaughtExceptionCounter ScopeGuardOnExit
#NO! PROGRAMMS+= ScopeGuardTest src/main unused/uncaught_exception unused/array_size
PROGRAMMS+= RangesSamples

SRC:=$(PROGRAMMS:=.cpp)
SRC+= src/ErrorHandler.cpp

OBJ:=$(SRC:.cpp=.o)
DEP:=$(OBJ:.o=.d)

.INTERMEDIATE: $(OBJ)
.PHONY: all build test check clean distclean
all: build # $(PROGRAMMS)

build:
	cmake -G Ninja -B $@ -S . -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
      -DCMAKE_FIND_USE_INSTALL_PREFIX=ON \
      -DCMAKE_PREFIX_PATH=$(CMAKE_STAGING_PREFIX) \
      -DCMAKE_STAGING_PREFIX=$(CMAKE_STAGING_PREFIX)
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

check: build
	run-clang-tidy -p $< -checks='-*,bugprone-*,hicpp-*,cpp-*,google-*,misc-*' \
    src #XXX $(SRC)
# OnLeavingScope.cpp ScopeGuardOnExit.cpp UncaughtExceptionCounter.cpp

clean:
	rm -rf $(PROGRAMMS) $(OBJ)

distclean: clean
	rm -rf build $(DEP)

-include $(DEP)
