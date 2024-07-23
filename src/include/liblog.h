/*
 * liblog.h - Personal C99 logging library.
 * See: https://github.com/8dcc/liblog
 * Copyright (C) 2024 8dcc
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of  MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LIBLOG_H_
#define LIBLOG_H_ 1

#include <stdio.h>

/*----------------------------------------------------------------------------*/
/* Settings */

/* Default FILE* when logging */
#ifndef LOG_FP
#define LOG_FP stderr
#endif

/* NOTE: Comment to disable ASCII colors when logging */
#define LOG_COLOR

/* NOTE: Comment to disable logging features */
/* #define LOG_DATE */
/* #define LOG_TIME */
#define LOG_TAG
#define LOG_FUNC

/*----------------------------------------------------------------------------*/
/* Enums */

enum ELogTag {
    LOG_TAG_DBG,
    LOG_TAG_INF,
    LOG_TAG_WRN,
    LOG_TAG_ERR,
    LOG_TAG_FTL,

    LOG_TAGS,
};

/*----------------------------------------------------------------------------*/
/* Functions and callable macros */

void log_write(enum ELogTag tag, const char* func, const char* fmt, ...);

#define log_dbg(...) log_write(LOG_TAG_DBG, __func__, __VA_ARGS__)
#define log_inf(...) log_write(LOG_TAG_INF, __func__, __VA_ARGS__)
#define log_wrn(...) log_write(LOG_TAG_WRN, __func__, __VA_ARGS__)
#define log_err(...) log_write(LOG_TAG_ERR, __func__, __VA_ARGS__)
#define log_ftl(...) log_write(LOG_TAG_FTL, __func__, __VA_ARGS__)

#endif /* LIBLOG_H_ */
