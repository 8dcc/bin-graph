#!/usr/bin/env bash
#
# Copyright 2025 8dcc. All Rights Reserved.
#
# This file is part of bin-graph.
#
# This program is free software: you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program.  If not, see <https://www.gnu.org/licenses/>.
#
# ------------------------------------------------------------------------------
#
# Parse the output of `readelf', and use the offsets of the specified section
# as arguments for `bin-graph'.
set -e

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
