//
//  file.h
//  file
//

#ifndef file_h
#define file_h

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
void readf(file_t* file, void* buffer, size_t bytes);
void rewindf(file_t* file);
int file_is_buffered(const file_t* file);
size_t file_length(const file_t* file);
void writef(file_t* file, void* buffer, size_t bytes);
void closef(const file_t* file);

#endif /* file_h */
