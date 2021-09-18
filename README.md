A fast and dirty checker which detects non-deterministic iteration e.g.
```
std::unordered_map<...> map;
for (auto [k, v] : map) {
  ...
}

std::map<Something *, ...> map;
for (auto [k, v] : map) {
  ...
}
```

Warnings are reported only if functions with predefined names (e.g. `printf`)
are encountered in a loop.

# How to build

To build, install (on Debian/Ubuntu) `llvm`, `libclang-dev` and `libpcre3-dev`:
```
$ sudo apt-get install llvm libclang-dev libpcre3-dev
```
and run
```
$ make clean all
```

To build debug or sanitized versions run `make` with `DEBUG=1`, `ASAN=1` or `UBSAN=1`.

# How to use

Run `bin/find-flaky` on files you are interested in:
```
$ bin/find-flaky tests/3-unordered-map-notok.cc
find-flaky: warning: calling IO function 'printf' inside flaky loop
```

By default tool looks for most common IO functions (like `printf`)
but you provide your own patterns:
```
$ cat patterns.txt
# A useful comment
my_debug_fun
# Another one
other_fun
$ bin/find-flaky --patterns patterns.txt ...
```

Custom compile flags (usually `-D` and `-I`) can be supplied with `--cflags` parameter and
verbosity can be set with `-v` flag.
For additional options run `bin/find-flaky -h`.

# How to test

Build and then run
```
$ make test
Found no errors
```

# TODO

* support other loop types
