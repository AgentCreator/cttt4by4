//
// Created by Oleksander Krainiak on 16.05.2026.
//
#include "../lib.c"
#include <time.h>
int main() {
    struct timespec start, end;
    struct Board b = {};
    struct HashTable *map = malloc( sizeof(struct HashTable));
    timespec_get(&start, TIME_UTC);
    solve(b, X, map);
    timespec_get(&end, TIME_UTC);
    printf("solved (in %fs)\n", (double)end.tv_sec - (double)start.tv_sec + (double)(end.tv_nsec - start.tv_nsec) * 1e-9);
    assert(end.tv_sec - start.tv_sec == 0); // 1 sec
    free(map);
}