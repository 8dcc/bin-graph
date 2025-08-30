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
# -----------------------------------------------------------------------------
#
# Simple script for generating multiple binary graphs from the same file.
set -e

BIN_GRAPH='bin-graph'

if [ $# -ne 1 ]; then
    echo "Usage: $(basename "$0") INPUT" 1>&2
    exit 1
fi

assert_cmd() {
    if [ ! "$(command -v "$1")" ]; then
        echo "$(basename "$0"): The '$1' command is not installed." 1>&2
        exit 1
    fi
}

assert_cmd "$BIN_GRAPH"

input_file="${*: -1}"

# Command-line options for each mode.
"$BIN_GRAPH" --mode 'grayscale' "$input_file" "${input_file}.grayscale.png"
"$BIN_GRAPH" --mode 'ascii' "$input_file" "${input_file}.ascii.png"
"$BIN_GRAPH" --mode 'entropy' --transform-squares 16 "$input_file" "${input_file}.entropy.png"
"$BIN_GRAPH" --mode 'histogram' "$input_file" "${input_file}.histogram.png"
"$BIN_GRAPH" --mode 'bigrams' "$input_file" "${input_file}.bigrams.png"
"$BIN_GRAPH" --mode 'dotplot' --offset-start 0 --offset-end 1000 "$input_file" "${input_file}.dotplot1.png"
"$BIN_GRAPH" --mode 'dotplot' --offset-start 4000 --offset-end 5000 "$input_file" "${input_file}.dotplot2.png"
"$BIN_GRAPH" --mode 'dotplot' --offset-start 10000 --offset-end 11000 "$input_file" "${input_file}.dotplot3.png"
"$BIN_GRAPH" --mode 'dotplot' --offset-start 30000 --offset-end 31000 "$input_file" "${input_file}.dotplot4.png"
