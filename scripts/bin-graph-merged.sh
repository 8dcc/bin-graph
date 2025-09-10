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

if [ $# -lt 2 ]; then
    echo "Usage: $(basename "$0") [OPTION...] INPUT OUTPUT.png" 1>&2
    exit 1
fi

assert_cmd() {
    if [ ! "$(command -v "$1")" ]; then
        echo "$(basename "$0"): The '$1' command is not installed." 1>&2
        exit 1
    fi
}

assert_cmd "$BIN_GRAPH"
assert_cmd 'magick'

get_width() {
    local image="$1"
    magick "$image" -format "%w" info:
}

get_height() {
    local image="$1"
    magick "$image" -format "%h" info:
}

max() {
    local max="$1"
    for num in "$@"; do
        if (( num > max )); then
            max="$num"
        fi
    done
    echo "$max"
}

bin_graph_args=("${@:1:$#-2}")
input_path="${*: -2:1}"
output_path="${*: -1:1}"
input_name="$(basename "$input_path")"

tmp_dir="$(mktemp --tmpdir --directory "bin-graph-merged.XXXXX")"

"$BIN_GRAPH" "${bin_graph_args[@]}" --mode 'grayscale' --width 256 "$input_path" "${tmp_dir}/${input_name}.grayscale.png"
"$BIN_GRAPH" "${bin_graph_args[@]}" --mode 'entropy-histogram' --width 64 --block-size 256 "$input_path" "${tmp_dir}/${input_name}.entropy-histogram.png"
"$BIN_GRAPH" "${bin_graph_args[@]}" --mode 'ascii' --width 256 --transform-hilbert 8 "$input_path" "${tmp_dir}/${input_name}.ascii.png"
"$BIN_GRAPH" "${bin_graph_args[@]}" --mode 'entropy' --width 256 --transform-hilbert 8 --block-size 256 "$input_path" "${tmp_dir}/${input_name}.entropy.png"
"$BIN_GRAPH" "${bin_graph_args[@]}" --mode 'histogram' --width 256 "$input_path" "${tmp_dir}/${input_name}.histogram.png"
"$BIN_GRAPH" "${bin_graph_args[@]}" --mode 'bigrams' "$input_path" "${tmp_dir}/${input_name}.bigrams.png"

#   +-----+-+-----+-----+---+
#   |     | |     |     | 5 |
#   |     | |     |     |---+
#   |  1  |2|  3  |  4  | 6 |
#   |     | |     |     |---+
#   |     | |     |     |
#   +-----+-+-----+-----+
#
# Target layout:
#
#   1. Grayscale.
#   2. Entropy histogram.
#   3. Hilbert ASCII.
#   4. Hilbert entropy.
#   5. Histogram.
#   6. Bigrams.

padding=5 # px

image1="${tmp_dir}/${input_name}.grayscale.png"
image2="${tmp_dir}/${input_name}.entropy-histogram.png"
image3="${tmp_dir}/${input_name}.ascii.png"
image4="${tmp_dir}/${input_name}.entropy.png"
image5="${tmp_dir}/${input_name}.histogram.png"
image6="${tmp_dir}/${input_name}.bigrams.png"

width1="$(get_width "$image1")"
width2="$(get_width "$image2")"
width3="$(get_width "$image3")"
width4="$(get_width "$image4")"
width5="$(get_width "$image5")"
width6="$(get_width "$image6")"

height1="$(get_height "$image1")"
height2="$(get_height "$image2")"
height3="$(get_height "$image3")"
height4="$(get_height "$image4")"
height5="$(get_height "$image5")"
height6="$(get_height "$image6")"

x1="$padding"
x2="$((x1 + width1 + padding))"
x3="$((x2 + width2 + padding))"
x4="$((x3 + width3 + padding))"
x5="$((x4 + width4 + padding))"
x6="$x5"

y1="$padding"
y2="$y1"
y3="$y2"
y4="$y3"
y5="$y4"
y6="$((y5 + height5 + padding))"

total_width="$((x5 + $(max "$width5" "$width6") + padding))"
total_height="$((y1 + $(max "$height1" "$height2" "$height3" "$height4" $((height5 + padding + height6))) + padding))"
magick -background transparent -size "${total_width}x${total_height}" xc:transparent \
  "$image1" -geometry +"$x1"+"$y1" -composite \
  "$image2" -geometry +"$x2"+"$y2" -composite \
  "$image3" -geometry +"$x3"+"$y3" -composite \
  "$image4" -geometry +"$x4"+"$y4" -composite \
  "$image5" -geometry +"$x5"+"$y5" -composite \
  "$image6" -geometry +"$x6"+"$y6" -composite \
  "$output_path"

if [ -d "$tmp_dir" ]; then
    rm -r "$tmp_dir"
fi
