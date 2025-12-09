#!/usr/bin/env bash
# Compare minishell behaviour against bash for key quoting/expansion cases.
# Produces per-case logs and diffs for debugging.
set -euo pipefail

WORKDIR=$(cd "$(dirname "$0")/.." && pwd)
MSH="$WORKDIR/minishell"
OUTDIR=/tmp/minishell_compare
mkdir -p "$OUTDIR"

run_case() {
  local name="$1"; shift
  local case_dir="$OUTDIR/$name"
  mkdir -p "$case_dir"
  local script="$case_dir/case.sh"
  printf "%s\n" "$@" > "$script"

  # Run under bash
  ( cd "$case_dir" && bash --noprofile --norc < "$script" \
    > "$case_dir/bash.out" 2> "$case_dir/bash.err" )
  local bash_status=$?

  # Run under minishell
  ( cd "$case_dir" && "$MSH" < "$script" \
    > "$case_dir/msh.out" 2> "$case_dir/msh.err" ) || true
  local msh_status=$?

  # Record statuses
  echo "$bash_status" > "$case_dir/bash.status"
  echo "$msh_status"  > "$case_dir/msh.status"

  # Diff outputs and errors; include status differences.
  {
    echo "== STATUS =="
    echo "bash: $bash_status"
    echo "msh : $msh_status"
    echo
    echo "== STDOUT DIFF (bash vs minishell) =="
    diff -u "$case_dir/bash.out" "$case_dir/msh.out" || true
    echo
    echo "== STDERR DIFF (bash vs minishell) =="
    diff -u "$case_dir/bash.err" "$case_dir/msh.err" || true
  } > "$case_dir/diff.log"

  echo "Case '$name' complete. See $case_dir"
}

echo "Running comparison suite; logs in $OUTDIR"

# Case: globbing and quoted/no-quote variants
run_case globbing \
  'echo *' \
  'echo "*"' \
  "echo '*'" \
  'exit'

# Case: expansion and adjacency
run_case expansion_adj \
  'VAR=foo' \
  'echo pre$VARpost' \
  'echo "a"$VAR' \
  'echo $VAR"b"' \
  "echo '$VAR'" \
  'exit'

# Case: quoted filenames in redirections
run_case redir_quotes \
  'echo hello > "file with spaces"' \
  'cat "file with spaces"' \
  'rm "file with spaces"' \
  'exit'

# Case: heredoc with quoted delimiter (no expansion expected)
run_case heredoc_quoted_delim \
  'VAR=zzz' \
  'cat <<'"'"'EOF'"'"'' \
  '$VAR' \
  'EOF' \
  'exit'

# Case: empty quotes and mixed joins
run_case empty_quotes \
  'echo ""' \
  "echo ''" \
  'echo "a""b"' \
  'exit'

echo "All comparison cases executed. Inspect per-case logs under $OUTDIR."

