//
//  file: file.h
//  Copyright (C) 2020 Ethan Uppal
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

#ifndef file_h
#define file_h

// Check platform

#if defined(_WIN32) || defined(_WIN64)
#define _WINDOWS
#elif defined(__unix__) || (defined (__APPLE__) && defined (__MACH__))
#ifndef _POSIX
#define _POSIX
#endif
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 0x200809L
#endif
#include <fcntl.h>
#endif

// Expose the necessary data types

typedef enum {
    FILE_NO_BUFFERING,
    FILE_BLOCK_SIZE_BUFFER,
    FILE_512_BYTE_BUFFER,
    FILE_256_BYTE_BUFFER
} file_buffering_mode_t;

typedef struct _file_t file_t;

// The attributes to be used in the `openf` function parsing segment.

#if defined(_WINDOWS)

#define FILEO_CREATE CREATE_ALWAYS
#define FILEO_TRUNC TRUNCATE_EXISTING

#define FILEO_RDONLY GENERIC_READ
#define FILEO_WRONLY GENERIC_WRITE
#define FILEO_RDWR (GENERIC_READ | GENERIC_WRITE)

#endif

#include <stddef.h>

// File manipulation

#ifdef _FILE_OPENF_OLD

#if defined(_POSIX)

#define FILEO_APPEND O_APPEND
#define FILEO_CREATE O_CREAT
#define FILEO_NONBLOCK O_NONBLOCK
#define FILEO_TRUNC O_TRUNC

#define FILEO_RDONLY O_RDONLY
#define FILEO_WRONLY O_WRONLY
#define FILEO_RDWR O_RDWR

#endif

file_t* openf(const char* filename, int flags, mode_t mode, file_buffering_mode_t buffer_mode);
#else
file_t* openf(const char* filename, const char* options, file_buffering_mode_t buffer_mode);
#endif
ssize_t readf(file_t* file, void* buffer, size_t bytes);
void rewindf(file_t* file);
ssize_t writef(file_t* file, void* buffer, size_t bytes);
void closef(const file_t* file);
void flushf(const file_t* file);

// Attribute querying/modification

int file_is_buffered(const file_t* file);
size_t file_length(const file_t* file);
size_t file_offset(const file_t* file);
int file_set_offset(file_t* file, size_t new_offset);

#if defined(_POSIX)
int file_flags(const file_t* file);
mode_t file_mode(const file_t* file);
#elif defined(_WINDOWS)
DWORD file_access(const file_t* file);
DWORD file_flags(const file_t* file);
#endif

#endif /* file_h */
