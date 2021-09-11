#!/bin/sh

set -eu
#set -x

cd $(dirname $0)

TOOL=$PWD/../bin/find-flaky

O=$(mktemp --suffix .out)
E=$(mktemp --suffix .err)
trap "rm -f $O $E" EXIT INT TERM

ERR=0
for t in *.cc; do
  if ! $TOOL $t >$O 2>$E; then
    ERR=$((ERR + 1))
    continue
  fi

  O_REF=$t.out
  if ! test -e $O_REF; then
    O_REF=/dev/null
  fi
  if ! diff $O_REF $O; then
    ERR=$((ERR + 1))
    continue
  fi

  E_REF=$t.err
  if ! test -e $E_REF; then
    E_REF=/dev/null
  fi
  if ! diff $E_REF $E; then
    ERR=$((ERR + 1))
    continue
  fi
done

if test $ERR -gt 0; then
  echo "Found $ERR errors"
else
  echo "Found no errors"
fi

exit $ERR
