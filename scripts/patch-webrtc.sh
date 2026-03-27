#!/bin/bash
# Apply patches to the downloaded WebRTC source.
# Skips patches that have already been applied (idempotent).
set -e

SOURCE_DIR="$1"
PATCHES_DIR="$2"

for patch in "$PATCHES_DIR"/webrtc-*.patch; do
  [ -f "$patch" ] || continue
  if git -C "$SOURCE_DIR" apply --check "$patch" 2>/dev/null; then
    echo "Applying patch: $(basename "$patch")"
    git -C "$SOURCE_DIR" apply "$patch"
  else
    echo "Skipping already-applied patch: $(basename "$patch")"
  fi
done
