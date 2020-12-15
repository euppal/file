//
//  file.c
//  file
//

#include "file.h"
#include <stdlib.h>

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
        default:
            break;
    }
    
    return file;
}

void readf(file_t* file, void* buffer, size_t bytes) {
    // Read if enough space
    if (file->_length - file->_offset >= bytes) {
        read(file->_fd, buffer, bytes);
        file->_offset += bytes;
    }
}

inline void rewindf(file_t* file) {
    file->_offset = 0;
}
inline int file_is_buffered(const file_t* file) {
    return file->_blk_buffer != NULL;
}
inline size_t file_length(const file_t* file) {
    return file->_length;
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
            if (bytes < file->_block_size) {
                write(file->_fd, buffer, bytes);
                bytes = 0;
            } else {
                write(file->_fd, buffer, file->_block_size);
                bytes -= file->_block_size;
            }
        }
    }
}

void closef(const file_t* file) {
    if (file->_blk_buffer != NULL) {
        write(file->_fd, file->_blk_buffer, file->_running_blksize);
        free(file->_blk_buffer);
    }
    close(file->_fd);
    free((void*)file);
}
