#!/usr/bin/env bash
set -euo pipefail
WORKDIR=$(cd "$(dirname "$0")/.." && pwd)
MSH="$WORKDIR/minishell"
OUTDIR=/tmp/minishell_migration_tests
mkdir -p "$OUTDIR"

echo "Running migration test suite; logs in $OUTDIR"

# Helper to run a sequence and capture log
run_case() {
  name="$1"
  shift
  printf "%s\n" "$@" | "$MSH" > "$OUTDIR/$name.out" 2>&1 || true
  echo "Wrote $OUTDIR/$name.out"
}

# Basic quoting/globbing
run_case basic 'echo *' 'echo "*"' "echo '*'" 'exit'

# Adjacent expansions and joins
run_case joins 'VAR=foo' 'echo pre$VARpost' 'echo bar$VAR' 'echo $VARbar' 'exit'

# Redirection with quoted filenames and spaces
run_case redirs 'echo "a" > "out quoted.txt"' 'echo foo > "file with spaces"' 'ls -l "out quoted.txt"' 'exit'

# Empty quotes and mixed joins
run_case empty_and_mixed 'echo ""' "echo ''" 'exit'

# Heredoc delimiter tests
cat > "$OUTDIR/heredoc_input.txt" <<'EOF'
cat <<"DELIM"
$SOMETHING
DELIM
exit
EOF
run_case heredoc "$(cat $OUTDIR/heredoc_input.txt)"

# Misc: variable adjacency with quotes
run_case adj_quotes 'VAR=foo' 'echo "a"$VAR' 'echo $VAR"b"' 'exit'


echo "All tests ran. Inspect files in $OUTDIR"
ls -l "$OUTDIR" || true
