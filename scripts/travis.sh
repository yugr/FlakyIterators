#!/bin/sh

# Copyright 2021-2022 Yury Gribov
# 
# Use of this source code is governed by MIT license that can be
# found in the LICENSE.txt file.

set -eu

if test -n "${TRAVIS:-}" -o -n "${GITHUB_ACTIONS:-}"; then
  set -x
fi

cd $(dirname $0)/..

make "$@" clean all

if test -n "${VALGRIND:-}"; then
  cp -r bin bin-real
  for f in $(find bin -type f -a -executable); do
    cat > $f <<EOF
#!/bin/sh
valgrind -q --error-exitcode=1 --suppressions=$PWD/scripts/valgrind.supp $PWD/bin-real/$(basename $f) "\$@"
EOF
    chmod +x $f
  done
fi

export ASAN_OPTIONS='detect_stack_use_after_return=1:check_initialization_order=1:strict_init_order=1:strict_string_checks=1'

make "$@" check

# Upload coverage
if test -n "${COVERAGE:-}"; then
  curl --retry 5 -s https://codecov.io/bash > codecov.bash
  bash codecov.bash -Z
fi
