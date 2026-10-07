#!/usr/bin/env bash
#
# Run repro.js under several Node versions (via fnm) and report how each exits.
#
#   test/process-exit/matrix.sh
#
# Versions must already be installed (`fnm install <major>`). Node is invoked
# directly rather than through `fnm exec`, because fnm exec reports a child
# killed by a signal as "Can't read exit code" with status 1.

set -u

cd "$(dirname "$0")/../.."

runs=10
majors=(16 17 18 19 20 21 22 23 24 25 26)

describe() {
  case $1 in
    0) echo "ok" ;;
    134) echo "SIGABRT" ;;
    139) echo "SIGSEGV" ;;
    *) echo "exit $1" ;;
  esac
}

printf "%-10s %-8s %s\n" "node" "scenario" "results ($runs runs)"

for major in "${majors[@]}"; do
  node_bin=$(fnm exec --using="$major" node -e 'process.stdout.write(process.execPath)' 2>/dev/null)
  if [ -z "$node_bin" ]; then
    printf "%-10s %s\n" "v$major" "not installed (fnm install $major)"
    continue
  fi
  version=$("$node_bin" -v)

  # process.exit() skips the teardown path that crashes, so this only checks
  # that the addon loads on this Node/arch.
  if ! load_err=$("$node_bin" -e 'require("./"); process.exit(0)' 2>&1); then
    reason=$(printf '%s\n' "$load_err" | grep -m1 '^Error' | cut -c1-80)
    printf "%-10s %s\n" "$version" "skipped: addon failed to load: $reason"
    continue
  fi

  for scenario in track closed; do
    results=()
    for _ in $(seq 1 "$runs"); do
      # The outer redirect hides bash's own "Segmentation fault" job report.
      { SCENARIO=$scenario "$node_bin" test/process-exit/repro.js > /dev/null 2>&1; code=$?; } 2>/dev/null
      results+=("$(describe $code)")
    done
    printf "%-10s %-8s %s\n" "$version" "$scenario" "${results[*]}"
  done
done
