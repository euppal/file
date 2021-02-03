//
//  file: file.c
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

#include "file.h"

#if defined(_POSIX)

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>

typedef struct _file_t {
    int _fd;
    int _flags;
    mode_t _mode;
    off_t _offset;
    off_t _length;
    blksize_t _block_size;
    blksize_t _running_blksize;
    char* _blk_buffer;
} file_t;

const char* filename_make_absolute(const char* filename) {
    static char buffer[PATH_MAX];

    if (filename[0] == '/') {
        return filename;
    } else {
        getcwd(buffer, sizeof(buffer));
        size_t end = strlen(buffer);
        buffer[end++] = '/';
        strcpy(buffer + end, filename);
        return buffer;
    }
}

#ifdef _FILE_OPENF_OLD
file_t* openf(const char* filename, int flags, mode_t mode, file_buffering_mode_t buffer_mode) {
    file_t* file = (file_t*)malloc(sizeof(file_t));
    
    // Open the file
    file->_fd = open(filename, flags, mode);
    file->_flags = flags;
    file->_mode = mode;
    
    // Setup information
    struct stat stat;
    fstat(file->_fd, &stat);
    file->_offset = 0;
    file->_length = stat.st_size;
    file->_block_size = stat.st_blksize;
    file->_running_blksize = 0;
    switch (buffer_mode) {
        case FILE_NO_BUFFERING:
            file->_blk_buffer = NULL;
            break;
        case FILE_BLOCK_SIZE_BUFFER:
            file->_blk_buffer = (char*)malloc(sizeof(char) * file->_block_size);
            break;
        case FILE_512_BYTE_BUFFER:
            file->_block_size = 512;
            file->_blk_buffer = (char*)malloc(sizeof(char) * 512);
            break;
        case FILE_256_BYTE_BUFFER:
            file->_block_size = 256;
            file->_blk_buffer = (char*)malloc(sizeof(char) * 256);
            break;
        default:
            break;
    }
    
    return file;
}
#else
static file_t* openf_fd(int fd, int flags, mode_t mode,
                        file_buffering_mode_t buffer_mode) {
    // Create the file handle
    file_t* file = malloc(sizeof(file_t));

    // Open the file
    file->_fd = fd;
    file->_flags = flags;
    file->_mode = mode;

    // Setup information
    struct stat stat;
    fstat(file->_fd, &stat);
    file->_offset = 0;
    file->_length = stat.st_size;
    file->_block_size = stat.st_blksize;
    file->_running_blksize = 0;
    switch (buffer_mode) {
        case FILE_NO_BUFFERING:
            file->_blk_buffer = NULL;
            break;
        case FILE_BLOCK_SIZE_BUFFER:
            file->_blk_buffer = (char*)malloc(sizeof(char) * file->_block_size);
            break;
        case FILE_512_BYTE_BUFFER:
            file->_block_size = 512;
            file->_blk_buffer = (char*)malloc(sizeof(char) * 512);
            break;
        case FILE_256_BYTE_BUFFER:
            file->_block_size = 256;
            file->_blk_buffer = (char*)malloc(sizeof(char) * 256);
            break;
    }

    return file;
}

file_t* openf(const char* filename, const char* options, file_buffering_mode_t buffer_mode) {
    mode_t mode = 0666;
    int flags = 0;

    // Traverse the string to get the options
    for (int i = 0; options[i]; i++) {
        switch (options[i]) {
            case 'r': {
                if (flags | FILEO_WRONLY) {
                    flags &= ~FILEO_WRONLY;
                    flags |= FILEO_RDWR;
                } else {
                    flags |= FILEO_RDONLY;
                }
                break;
            }
            case 'w': {
                if (flags | FILEO_RDONLY) {
                    flags &= ~FILEO_RDONLY;
                    flags |= FILEO_RDWR;
                } else {
                    flags |= FILEO_WRONLY;
                }
                break;
            }
            case 'c': {
                flags |= FILEO_CREATE;
                break;
            }
            case 'a': {
                flags |= FILEO_APPEND;
                break;
            }
            case 't': {
                flags |= FILEO_TRUNC;
                break;
            }
            default:
                break;
        }
    }
    filename = filename_make_absolute(filename);
    int fd = open(filename, flags, mode);
    if (fd < 0) {
        return NULL;
    }
    return openf_fd(fd, flags, mode, buffer_mode);
}
#endif

size_t readf(file_t* file, void* buffer, size_t bytes) {
    // If the remaining bytes < requested bytes, simply read only the remaining bytes
    if ((size_t)(file->_length - file->_offset) < bytes) {
        bytes = file->_length - file->_offset;
    }
    
    read(file->_fd, buffer, bytes);
    file->_offset += bytes;
    return bytes;
}

inline void rewindf(file_t* file) {
    file->_offset = 0;
}

void writef(file_t* file, void* buffer, size_t bytes) {
    if (file_is_buffered(file)) {
        size_t i = 0;
        while (i < bytes) {
            if (file->_running_blksize >= file->_block_size) {
                write(file->_fd, buffer, file->_block_size);
                file->_running_blksize = 0;
            } else {
                file->_blk_buffer[file->_running_blksize++] = ((char*)buffer)[i++];
            }
        }
    } else {
        while (bytes > 0) {
            if (bytes < (size_t)file->_block_size) {
                write(file->_fd, buffer, bytes);
                return;
            } else {
                write(file->_fd, buffer, file->_block_size);
                bytes -= file->_block_size;
            }
        }
    }
}

inline void flushf(const file_t* file) {
    if (file->_blk_buffer != NULL) {
        if (file->_running_blksize > 0) {
            write(file->_fd, file->_blk_buffer, file->_running_blksize);
        }
        free(file->_blk_buffer);
    }
}

void closef(const file_t* file) {
    flushf(file);
    close(file->_fd);
    free((void*)file);
}

inline int file_is_buffered(const file_t* file) {
    return file->_blk_buffer != NULL;
}
inline size_t file_length(const file_t* file) {
    return file->_length;
}
inline size_t file_offset(const file_t* file) {
    return file->_offset;
}
inline int file_set_offset(file_t* file, size_t new_offset) {
    if (new_offset < (size_t)file->_length) {
        file->_offset = new_offset;
        return 0;
    }
    return 1;
}
inline int file_flags(const file_t* file) {
    return file->_flags;
}
inline mode_t file_mode(const file_t* file) {
    return file->_mode;
}

#elif defined(_WINDOWS)

#include <Windows.h>

typedef struct _file_t {
    HANDLE _handle;
    DWORD _access;
    DWORD _flags;
    size_t _length;
    size_t _offset;
    DWORD _block_size;
    DWORD _running_blksize;
    char* _blk_buffer;
} file_t;

file_t* openf(const char* filename, const char* options, file_buffering_mode_t buffer_mode) {
    DWORD access = 0;
    DWORD flags = 0;

    // Traverse the string to get the options
    for (int i = 0; options[i]; i++) {
        switch (options[i]) {
            case 'r': {
                access |= FILEO_RDONLY;
                break;
            }
            case 'w': {
                access |= FILEO_WRONLY;
                break;
            }
            case 'c': {
                flags |= FILEO_CREATE;
                break;
            }
            case 't': {
                flags |= FILEO_TRUNC;
                break;
            }
            default:
                break;
        }
    }

    // Create the file handle
    file_t* file = (file_t*)malloc(sizeof(file_t));
    file->_access = access;
    file->_flags = flags;
    
    // Get file information
    DWORD high_length;
    DWORD low_length = GetFileSize(file->_handle, &high_length);
    file->_length = (size_t)high_length << 32 | (size_t)low_length;
    file->_running_blksize = 0;
    file->_block_size = 4096//good windows block size;
    file->_running_blksize = 0;
    switch (buffer_mode) {
        case FILE_NO_BUFFERING:
            file->_blk_buffer = NULL;
            break;
        case FILE_BLOCK_SIZE_BUFFER:
            file->_blk_buffer = (char*)malloc(sizeof(char) * file->_block_size);
            break;
        case FILE_512_BYTE_BUFFER:
            file->_block_size = 512;
            file->_blk_buffer = (char*)malloc(sizeof(char) * 512);
            break;
        case FILE_256_BYTE_BUFFER:
            file->_block_size = 256;
            file->_blk_buffer = (char*)malloc(sizeof(char) * 256);
            break;
        default:
            break;
    }
    
    return file;
}

size_t readf(file_t* file, void* buffer, size_t bytes) {
    size_t bytes_read = 0;
    if (file->_offset < file->_length) {
        ReadFile(file->_handle, buffer, bytes, &bytes_read, NULL);
        file->_offset += bytes_read;
    }
    return bytes_read;
}

inline void rewindf(file_t* file) {
    file->_offset = 0;
}

void writef(file_t* file, void* buffer, size_t bytes) {
    size_t bytes_written;
    if (file_is_buffered(file)) {
        size_t i = 0;
        while (i < bytes) {
            if (file->_running_blksize >= file->_block_size) {
                WriteFile(file->_handle, buffer, file->_block_size, &bytes_written, NULL);
                file->_running_blksize = 0;
            } else {
                file->_blk_buffer[file->_running_blksize++] = ((char*)buffer)[i++];
            }
        }
    } else {
        while (bytes > 0) {
            if (bytes < file->_block_size) {
                WriteFile(file->_handle, buffer, bytes, &bytes_written, NULL);
                return;
            } else {
                WriteFile(file->_handle, buffer, file->_block_size, &bytes_written, NULL);
                bytes -= file->_block_size;
            }
        }
    }
}

void closef(const file_t* file) {
    if (file->_blk_buffer != NULL) {
        if (file->_running_blksize > 0) {
            //write(file->_fd, file->_blk_buffer, file->_running_blksize);
        }
        free(file->_blk_buffer);
    }
    CloseHandle(file->_handle);
    free((void*)file);
}

inline int file_is_buffered(const file_t* file) {
    return file->_blk_buffer != NULL;
}
inline size_t file_length(const file_t* file) {
    return file->_length;
}
inline size_t file_offset(const file_t* file) {
    return file->_offset;
}
inline int file_set_offset(file_t* file, size_t new_offset) {
    if (new_offset < (size_t)file->_length) {
        file->_offset = new_offset;
        return 0;
    }
    return 1;
}
inline DWORD file_access(const file_t* file) {
    return file->_access;
}
inline DWORD file_flags(const file_t* file) {
    return file->_flags;
}

#endif
