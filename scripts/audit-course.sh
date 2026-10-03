#!/bin/sh
set -eu

fail=0

for n in $(seq -w 0 19); do
  matches=$(find course -maxdepth 1 -type f -name "${n}-*.md" | wc -l)
  if [ "$matches" -ne 1 ]; then
    echo "expected exactly one course chapter ${n}, found $matches" >&2
    fail=1
  fi
done

if find course -maxdepth 1 -type f -name '[0-9][0-9]-*.md' | grep -Ev '/(0[0-9]|1[0-9])-.*\.md$' >/dev/null; then
  echo "unexpected numbered course chapter outside 00-19" >&2
  fail=1
fi

grep -q 'course/00-learning-path.md' README.md
grep -q 'W65C816' course/README.md
grep -q 'Physical PASS is never inferred from emulator success' course/README.md
grep -q 'M6 status: IN PROGRESS' ROADMAP.md
grep -q 'M7 status: COMPLETE' ROADMAP.md
grep -q 'M8 status: IN PROGRESS' ROADMAP.md

test "$fail" -eq 0
echo "Course structure audit: PASS"
