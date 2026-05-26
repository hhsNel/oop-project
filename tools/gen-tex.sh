#!/bin/bash

set -euox pipefail

PROOT="$(pwd | sed 's|/tools$||')"
TOOL="$PROOT/tools/im2bk"

declare -A TEX_DIRS=(
    [cultured-tex]=cultured-textures
    [kp-tex]=kp-textures
    [common]=common
)

for src_dir in "${!TEX_DIRS[@]}"; do
    dst_dir="${TEX_DIRS[$src_dir]}"
    CT_SRC="$(find "$PROOT/raw-res/$src_dir/" -type f -name "*.png")"
    for img in $CT_SRC; do
        dst="$(echo "$img" | sed "s|raw-res/$src_dir/|res/$dst_dir/|g" | sed 's/\.png$/.btx/g')"
        "$TOOL" "$img" "$dst"
    done
done

