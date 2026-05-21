#!/bin/sh
set -ex

output="$1"
resdir="$2"

cat > "$output" << 'EOF'
#pragma once
namespace res_symbols {
extern "C" {
EOF

find "$resdir" -type f | while read -r file; do
    rel_path=$(echo "$file" | sed "s|^$resdir/||")
    name=$(echo "$rel_path" | sed 's/[^a-zA-Z0-9]/_/g')

    cat >> "$output" << EOF
    extern unsigned char const _binary_res_${name}_start[];
    extern unsigned char const _binary_res_${name}_end[];
EOF
done

cat >> "$output" << 'EOF'
}
}
EOF

