#!/usr/bin/env bash

# Parse the output of `readelf', and use the offsets of the specified section
# as arguments for `bin-graph'.

if [ $# -lt 3 ]; then
    echo "Usage: $(basename "$0") SECTION [OPTION...] INPUT OUTPUT.png" 1>&2
    exit 1
fi

input_section=$1
input_file="${*: -2:1}"

readelf_output="$(readelf --sections --wide "$input_file" | grep --fixed-strings "$input_section" | head -n 1 | tr -s ' ')"

if [ -z "$readelf_output" ]; then
    echo "$(basename "$0"): Section '${input_section}' not found." 1>&2
    exit 1
fi

section_name=$(echo "$readelf_output" | cut -d ' ' -f 3)
echo "Section name: ${section_name}"

offset_start_str=$(echo "$readelf_output" | cut -d ' ' -f 6)
offset_start=$(printf '%x' "0x${offset_start_str}")
echo "Start offset: 0x${offset_start}"

section_size=$(echo "$readelf_output" | cut -d ' ' -f 7)
offset_end=$(printf '%x' $((0x${offset_start} + 0x${section_size} + 1)))
echo "End offset: 0x${offset_end}"

cmd='./bin-graph'
args=(--offset-start "$offset_start" --offset-end "$offset_end" "${@:2}")
echo "Running: ${cmd} ${args[*]}"
$cmd "${args[@]}"
