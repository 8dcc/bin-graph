/*
 * Copyright 2025 8dcc
 *
 * This file is part of bin-graph.
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef EXPORT_H_
#define EXPORT_H_ 1

#include <stdbool.h>
#include <stdio.h> /* FILE */

#include "image.h"

/*
 * Pointer to a function that exports an 'Image' into an output file.
 */
typedef bool (*export_func_ptr_t)(const Args* args,
                                  const Image* image,
                                  FILE* output_fp);

/*
 * Export the specified 'Image' structure into the specified PNG file.
 */
bool export_png(const Args* args, const Image* image, FILE* output_fp);

/*----------------------------------------------------------------------------*/

/*
 * Return a pointer to the export function associated to a specific mode.
 */
static inline export_func_ptr_t export_func_from_output_format(
  enum EArgsOutputFormat format) {
    switch (format) {
        case ARGS_OUTPUT_FORMAT_PNG:
            return export_png;
    }
    return NULL;
}

#endif /* EXPORT_H_ */
