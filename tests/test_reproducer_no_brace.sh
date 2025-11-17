#!/bin/bash
# Small reproducible test for minishell variable expansion behavior
# Project intentionally does NOT support ${} brace expansion or backslash escapes.
set -euo pipefail
# Run minishell and capture both stdout and stderr to a logfile for parsing
cd "$(dirname "$0")/.."
LOG="tests/reproducer.log"
rm -f "$LOG"
./minishell > "$LOG" 2>&1 <<'EOF'
export VAR=val
echo "start$VARend"
echo "\"quoted\""
exit
EOF
OUT=$(cat "$LOG")
echo "Captured output written to $LOG"
# Check for exact lines 'start' and '\quoted\'
if echo "$OUT" | grep -Fxq 'start'; then
    echo "PASS: found line 'start'"
else
    echo "FAIL: expected line 'start' not found"
    echo "=== RAW OUTPUT ==="
    echo "$OUT"
    exit 1
fi
expected=$'\\quoted\\'
found=0
while IFS= read -r line; do
    if [ "$line" = "$expected" ]; then
        found=1
        break
    fi
done <<< "$OUT"
if [ "$found" -eq 1 ]; then
    echo "PASS: found line '$expected'"
else
    echo "FAIL: expected line '$expected' not found"
    echo "=== RAW OUTPUT ==="
    echo "$OUT"
    exit 1
fi
echo "All checks passed." 
