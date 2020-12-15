//
//  main.c
//  file
//

// This main.c is for TESTING.

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "file.h"

long test_file() {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    file_t* file = openf("test1", O_CREAT | O_RDWR, 0666, FILE_512_BYTE_BUFFER);
    char str[1] = { 'a' };
    for (size_t i = 0; i < 8192; i++) {
        writef(file, str, 1);
    }
    closef(file);
    clock_gettime(CLOCK_MONOTONIC, &end);
    return end.tv_nsec - start.tv_nsec;
}
long test_stdio() {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    FILE* file = fopen("test2", "w");
    char str[1] = { 'a' };
    for (size_t i = 0; i < 8192; i++) {
        fwrite(str, sizeof(char), 1, file);
    }
    fclose(file);
    clock_gettime(CLOCK_MONOTONIC, &end);
    return end.tv_nsec - start.tv_nsec;
}

int main(int argc, const char * argv[]) {
    printf("5 TRIALS\n"
           "--------\n");
    for (size_t i = 1; i <= 5; i++) {
        printf("Trial %zu:\n", i);
        printf("  file.h: %ld ms\n", test_file() / 100000);
        printf("  stdio.h: %ld ms\n", test_stdio() / 100000);
        system("rm test1; rm test2");
    }
    return 0;
}
