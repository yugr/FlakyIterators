# The MIT License (MIT)
# 
# Copyright (c) 2021-2022 Yury Gribov
# 
# Use of this source code is governed by The MIT License (MIT)
# that can be found in the LICENSE.txt file.

$(shell mkdir -p bin)

LLVM_CONFIG ?= llvm-config
DESTDIR ?= /usr/local/bin

CXX ?= g++

CXXFLAGS = $(shell $(LLVM_CONFIG) --cflags) -std=c++17 -g -Wall -Wextra -Werror
LDFLAGS = $(shell $(LLVM_CONFIG) --ldflags)
LIBS = -lclang -lpcrecpp

ifneq (,$(COVERAGE))
  DEBUG = 1
  CXXFLAGS += --coverage -DNDEBUG
  LDFLAGS += --coverage
endif
ifeq (,$(DEBUG))
  CXXFLAGS += -O2
  LDFLAGS += -Wl,-O2
else
  CXXFLAGS += -O0
endif
ifneq (,$(ASAN))
  CXXFLAGS += -fsanitize=address -fsanitize-address-use-after-scope -U_FORTIFY_SOURCE -fno-common -D_GLIBCXX_DEBUG -D_GLIBCXX_SANITIZE_VECTOR
  LDFLAGS += -fsanitize=address
endif
ifneq (,$(UBSAN))
  CXXFLAGS += -fsanitize=undefined,integer
  LDFLAGS += -fsanitize=undefined,integer
endif

all: bin/find-flaky

install:
	install bin/find-flaky $(DESTDIR)

bin/find-flaky: bin/main.o bin/utils.o bin/analysis.o bin/error.o bin/clang-utils.o
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

bin/%.o: src/%.cc
	$(CXX) $(CXXFLAGS) -o $@ -c $^

bin/%.o: src/analysis.h src/utils.h src/error.h src/clang-utils.h

test:
	tests/run.sh

clean:
	rm -f bin/*

.PHONY: all install clean test
