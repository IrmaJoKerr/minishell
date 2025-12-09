#!/usr/bin/env bash
# Mixed single/double quote and expansion comparison between bash and minishell.
set -euo pipefail

WORKDIR=$(cd "$(dirname "$0")/.." && pwd)
MSH="$WORKDIR/minishell"
OUTDIR=/tmp/minishell_mixed_quotes
mkdir -p "$OUTDIR"
# Avoid set -u aborts on VAR usage in scripts
VAR="__placeholder__"

run_case() {
  local name="$1"; shift
  local case_dir="$OUTDIR/$name"
  mkdir -p "$case_dir"
  local script="$case_dir/case.sh"
  printf "%s\n" "$@" > "$script"

  ( cd "$case_dir" && bash --noprofile --norc < "$script" \
    > bash.out 2> bash.err )
  local bash_status=$?

  ( cd "$case_dir" && "$MSH" < "$script" \
    > msh.out 2> msh.err ) || true
  local msh_status=$?

  if [ -f "$case_dir/bash.out" ] && [ -f "$case_dir/msh.out" ]; then
    {
      echo "== STATUS ==";
      echo "bash: $bash_status";
      echo "msh : $msh_status";
      echo;
      echo "== STDOUT DIFF (bash vs minishell) ==";
      diff -u "$case_dir/bash.out" "$case_dir/msh.out" || true;
      echo;
      echo "== STDERR DIFF (bash vs minishell) ==";
      diff -u "$case_dir/bash.err" "$case_dir/msh.err" || true;
    } > "$case_dir/diff.log"
  else
    {
      echo "== STATUS ==";
      echo "bash: $bash_status";
      echo "msh : $msh_status";
      echo;
      echo "Missing output files; bash.out/msh.out not found";
    } > "$case_dir/diff.log"
  fi

  echo "Case '$name' complete. See $case_dir"
}

echo "Running mixed quotes comparison suite; logs in $OUTDIR"

# Mixed adjacency and expansions (using environment vars)
run_case adj_mix \
  'echo "a"$HOME"b"' \
  "echo 'a'$HOME'b'" \
  'echo "a"$HOME' \
  'echo $HOME"b"' \
  "echo 'a'$HOME" \
  'echo $$' \
  'echo $' \
  'echo $USER' \
  'exit'

# Nested-looking patterns (not true nesting, just adjacent quotes)
run_case pseudo_nested \
  'echo "x"'"'"'y'"'"'"z"' \
  'echo "\"d$HOME\""' \
  "echo 'd$HOME'" \
  'echo "\"$USER\"\"$HOME\""' \
  'exit'

# Empty quotes mixed with expansions
run_case empty_mix \
  'echo ""$HOME""' \
  "echo ''$HOME''" \
  'echo """"' \
  "echo '' ''" \
  'echo $' \
  'exit'

# Redirections with mixed quoted filenames
run_case redir_mix \
  'echo data > "file'"'"'\"mix\"'"'"'.txt"' \
  'cat "file'"'"'\"mix\"'"'"'.txt"' \
  'rm "file'"'"'\"mix\"'"'"'.txt"' \
  'exit'

# Heredoc with mixed quote content but unquoted delimiter (expansion expected)
run_case heredoc_mixed \
  'cat <<EOF' \
  '"$HOME"' \
  "'$HOME'" \
  '$$' \
  '$' \
  '$USER' \
  'EOF' \
  'exit'

# Heredoc with quoted delimiter (no expansion)
run_case heredoc_quoted_delim \
  'cat <<'"'"'EOF'"'"'' \
  '"$HOME"' \
  "'$HOME'" \
  '$$' \
  '$' \
  '$USER' \
  'EOF' \
  'exit'

echo "All mixed quotes cases executed. Inspect per-case logs under $OUTDIR."

