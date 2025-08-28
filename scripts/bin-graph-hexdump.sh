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
# Print the output of a 'hexdump' command side by side with the ASCII output of
# 'bin-graph'.
set -e

HEXDUMP='xxd'
HEXDUMP_ARGS=(-R always)
BIN_GRAPH='bin-graph'
BIN_GRAPH_ARGS=(--width 16 --zoom 2 --output-format 'escaped-text')
PASTE='paste'

if [ $# -lt 1 ]; then
    echo "Usage: $(basename "$0") [OPTION...] INPUT" 1>&2
    exit 1
fi

assert_cmd() {
    if [ ! "$(command -v "$1")" ]; then
        echo "$(basename "$0"): The '$1' command is not installed." 1>&2
        exit 1
    fi
}

assert_cmd "$HEXDUMP"
assert_cmd "$BIN_GRAPH"
assert_cmd "$PASTE"

# Last argument must be the input file.
input_file="${*: -1}"

# Final string of arguments for 'bin-graph'.
bin_graph_args=("${BIN_GRAPH_ARGS[@]}" "${@:1:$#-1}")

# In order to redirect the standard input to both commands, it needs to be
# buffered in a temporary file.
if [ "$input_file" = "-" ]; then
    tmp_file="$(mktemp --tmpdir "bin-graph-hexdump.XXXXX.bin")"
    cat "$input_file" > "$tmp_file"
    input_file="$tmp_file"
fi

# Merge the output of the hexdump command with the ANSI-encoded output of
# 'bin-graph'.
#
# FIXME: If the last hexdump line is partial, the output of 'bin-graph' is not
# aligned.
"$PASTE" <("$HEXDUMP" "${HEXDUMP_ARGS[@]}" "$input_file") \
         <("$BIN_GRAPH" "${bin_graph_args[@]}" "$input_file" '-')

# We are done with the temporary file, delete it.
if [ -n "$tmp_file" ]; then
    rm "$tmp_file"
fi
