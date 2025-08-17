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

#include "image.h"

/*
 * Export the specified 'Image' structure into a PNG file with the specified
 * name, using 'zoom' for scaling.
 *
 * TODO: Return boolean instead of using 'DIE'.
 */
void export_png(Image* image, const char* filename, int zoom);

#endif /* EXPORT_H_ */
