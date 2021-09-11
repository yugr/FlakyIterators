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

To build, install `libclang-dev` and run
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

Custom compile flags can be supplied with `--cflags` parameter and
verbosity can be set with `-v` flag.
For additional options run `bin/find-flaky -h`.

# How to test

Build and then run
```
$ make test
Found no errors
```

# TODO

* read functions from file
* support other loop types
