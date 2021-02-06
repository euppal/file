//
//  main.c
//  This main.c is for TESTING.
//

#include <stdio.h>
#include <stdlib.h>
#include "file.h"

int main() {
    file_t* file = openf("file.h", "wr", FILE_NO_BUFFERING);
    writef(file, "Hello", 5);
    writef(file, ", world!", 8);
    flushf(file);
    char* contents = malloc(file_length(file) + 1);
    contents[file_length(file)] = 0;
    readf(file, contents, file_length(file));
    puts(contents);
    closef(file);
    return 0;
}
