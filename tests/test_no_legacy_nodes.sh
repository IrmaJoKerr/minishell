#!/bin/bash
# Detect creation of legacy token types (single-quote/double-quote/expansion)
set -euo pipefail
cd "$(dirname "$0")/.."
LOG="tests/legacy_nodes.log"
rm -f "$LOG"

./minishell > "$LOG" 2>&1 <<'EOF'
# Basic cases that previously triggered legacy nodes
echo $HOME
export VAR=val
echo "$VAR"
echo '$VAR'
echo ""quoted""
unset UNDEF
echo "$UNDEF"
export A=a
export B=b
echo "$A$B"
exit
EOF

OUT=$(cat "$LOG")
echo "Captured output written to $LOG"

# Fail if any legacy initnode token creation was logged
if echo "$OUT" | grep -Fq 'DEBUG TOK: initnode created type='; then
    echo "FAIL: legacy token creation detected"
    echo "=== OFFENDING LINES ==="
    echo "$OUT" | grep -F 'DEBUG TOK: initnode created type='
    exit 1
fi

echo "PASS: no legacy initnode creations detected"
exit 0
