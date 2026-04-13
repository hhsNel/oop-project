#!/bin/sh
set -ex

output="$1"
resdir="$2"

cat > "$output" << 'EOF'
#pragma once
#define RES_H

#ifdef __cplusplus
extern "C" {
#endif

EOF

for file in "$resdir"/*; do
	if [ -f "$file" ]; then
		name=$(basename "$file" | sed 's/[^a-zA-Z0-9]/_/g')
		cat >> "$output" << EOF
extern unsigned char const ${name}_start[];
extern unsigned char const ${name}_end[];
EOF
	fi
done

cat >> "$output" << 'EOF'

#ifdef __cplusplus
}
#endif
EOF

