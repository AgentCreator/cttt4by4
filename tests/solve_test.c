//
// Created by Oleksander Krainiak on 15.05.2026.
//
#include "../lib.c"
int main() {
    struct Board b = {0, 0};
    struct HashTable *map = calloc(1, sizeof(struct HashTable));
    b.mask =  0b1111'1111'1111'0000;
    b.moves = 0b1110'0010'1110'0000;
    solve(b, X, map);
    int n = bestMove(map, b, X, nullptr);
    assert(n == 14);

    free(map);
}