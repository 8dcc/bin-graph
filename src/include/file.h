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

#ifndef FILE_H_
#define FILE_H_ 1

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h> /* FILE */

#include "byte_array.h"

/*
 * Enumeration with the available modes for opening files.
 */
enum EFileOpenMode {
    FILE_MODE_READ,
    FILE_MODE_WRITE,
};

/*----------------------------------------------------------------------------*/

/*
 * Open the file at the specified path with the specified mode. This function
 * supports special strings for specifying 'stdin' and 'stdout', defined in
 * 'file.c'.
 *
 * The caller is responsible for checking if this function returns NULL in case
 * of error.
 */
FILE* file_open(const char* path, enum EFileOpenMode mode);

/*
 * Read the bytes of a file in a linear way from the starting offset to the end
 * offset. This function returns true on success, or false otherwise.
 *
 * Note that this function expects the file position to be on the first byte,
 * that is, the 'offset_start' and 'offset_end' arguments are actually relative
 * to the current file position.
 */
bool file_read(ByteArray* dst,
               FILE* fp,
               size_t offset_start,
               size_t offset_end);

#endif /* FILE_H_ */
