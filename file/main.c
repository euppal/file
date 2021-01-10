//
//  main.c
//  This main.c is for TESTING.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "file.h"
#define TRIALS 5
#define SIZE 0x100000ULL

long test_file() {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    file_t* file = openf("test1", "cw", FILE_512_BYTE_BUFFER);
    char str[1] = { 'a' };
    for (size_t i = 0; i < SIZE; i++) {
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
    for (size_t i = 0; i < SIZE; i++) {
        fwrite(str, sizeof(char), 1, file);
    }
    fclose(file);
    clock_gettime(CLOCK_MONOTONIC, &end);
    return end.tv_nsec - start.tv_nsec;
}

int main(int argc, const char * argv[]) {
    printf("%d TRIALS\n"
           "--------\n", TRIALS);
    long total_file = 0;
    long total_stdio = 0;
    for (int i = 1; i <= TRIALS; i++) {
        const long file_time = test_file() / 100000;
        total_file += file_time;
        printf("Trial %d:\n", i);
        printf("  file.h: %ld ms\n", file_time);
        const long stdio_time = test_stdio() / 100000;
        total_stdio += stdio_time;
        printf("  stdio.h: %ld ms\n", stdio_time);
        system("rm test1; rm test2");
    }
    printf("AVG file.h: %ld ms, AVG stdio.h: %ld ms\n", total_file / TRIALS, total_stdio / TRIALS);
    return 0;
}
