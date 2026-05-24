#!/bin/bash

set -euox pipefail

PROOT="$(pwd | sed 's|/tools$||')"
TOOL="$PROOT/tools/im2bk"

for as in cultured kp; do
	CT_SRC="$(find "$PROOT/raw-res/$as-tex/" -type f -name "*.png")"
	for img in $CT_SRC; do
		dst="$(echo "$img" | sed "s|raw-res/$as-tex/|res/$as-textures/|g" | sed 's/\.png$/.btx/g')"
		"$TOOL" "$img" "$dst"
	done
done

