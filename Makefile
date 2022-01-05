# The MIT License (MIT)
# 
# Copyright (c) 2021-2022 Yury Gribov
# 
# Use of this source code is governed by The MIT License (MIT)
# that can be found in the LICENSE.txt file.

$(shell mkdir -p bin)

CXX ?= g++
LLVM_CONFIG ?= llvm-config
DESTDIR ?= /usr/local

CXXFLAGS = $(shell $(LLVM_CONFIG) --cflags) -std=c++17 -g -Wall -Wextra -Werror
LDFLAGS = $(shell $(LLVM_CONFIG) --ldflags) -Wl,--warn-common
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
  ifneq (,$(shell $(CXX) --version | grep clang))
    # Isan is clang-only...
    CXXFLAGS += -fsanitize=undefined,integer -fno-sanitize-recover=undefined,integer
    LDFLAGS += -fsanitize=undefined,integer -fno-sanitize-recover=undefined,integer
  else
    CXXFLAGS += -fsanitize=undefined -fno-sanitize-recover=undefined
    LDFLAGS += -fsanitize=undefined -fno-sanitize-recover=undefined
  endif
endif

all: bin/find-flaky

install:
	mkdir -p $(DESTDIR)
	install bin/find-flaky $(DESTDIR)/bin

bin/find-flaky: bin/main.o bin/utils.o bin/analysis.o bin/error.o bin/clang-utils.o Makefile bin/FLAGS
	$(CXX) $(LDFLAGS) -o $@ $(filter %.o, $^) $(LIBS)

bin/%.o: src/%.cc Makefile bin/FLAGS
	$(CXX) $(CXXFLAGS) -o $@ -c $<

bin/%.o: src/analysis.h src/utils.h src/error.h src/clang-utils.h

bin/FLAGS: FORCE
	if test x"$(CXXFLAGS) $(LDFLAGS)" != x"$$(cat $@)"; then \
		echo "$(CXXFLAGS) $(LDFLAGS)" > $@; \
	fi

test:
	tests/run.sh

clean:
	rm -f bin/*
	find -name \*.gcov -o -name \*.gcno -o -name \*.gcda | xargs rm -rf

.PHONY: all install clean test FORCE
