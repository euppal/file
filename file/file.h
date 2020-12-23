//
//  file.h
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

#define _POSIX_C_SOURCE 0x200809L
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

typedef enum {
    FILE_NO_BUFFERING,
    FILE_BLOCK_SIZE_BUFFER,
    FILE_512_BYTE_BUFFER
} file_buffering_mode_t;

typedef struct {
    int _fd;
    int _flags;
    mode_t _mode;
    off_t _offset;
    off_t _length;
    blksize_t _block_size;
    blksize_t _running_blksize;
    char* _blk_buffer;
} file_t;

file_t* openf(const char* filename, int flags, mode_t mode, file_buffering_mode_t buffer_mode);
size_t readf(file_t* file, void* buffer, size_t bytes);
void rewindf(file_t* file);
int file_is_buffered(const file_t* file);
size_t file_length(const file_t* file);
size_t file_offset(const file_t* file);
void writef(file_t* file, void* buffer, size_t bytes);
void closef(const file_t* file);

#endif /* file_h */
