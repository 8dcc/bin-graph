#!/bin/bash

# Parse the output of `readelf', and use the offsets of the specified section
# as arguments for `bin-graph'.

if [ $# -lt 3 ]; then
    echo "Usage: $0 SECTION [OPTIONS...] INPUT OUTPUT.png"
    exit 1
fi

section=$1
input=${@: -2:1}

readelf_output=$(readelf --sections --wide "$input" | grep --fixed-strings "$section" | head -n 1 | tr -s ' ')

if [ -z "$readelf_output" ]; then
    echo "Section \"$section\" not found"
    exit 1
fi

section_name=$(echo "$readelf_output" | cut -d ' ' -f 3)
start_offset=$(echo "$readelf_output" | cut -d ' ' -f 6)
section_size=$(echo "$readelf_output" | cut -d ' ' -f 7)
end_offset=$(printf '%x' $((0x$start_offset + 0x$section_size + 1)))

echo "Section name: $section_name"
echo "Start offset: 0x$start_offset"
echo "End offset: 0x$end_offset"

echo "Running: ./bin-graph --offsets \"$start_offset-$end_offset\" ${@:2}"
./bin-graph --offsets "$start_offset-$end_offset" ${@:2}
