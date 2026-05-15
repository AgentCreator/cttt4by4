//
// Created by Oleksander Krainiak on 15.05.2026.
//
#include "../lib.c"

int main() {
    struct Board b = {0, 0};
    struct HashTable *map = calloc(1, sizeof(struct HashTable));
    // solve(b, X, map);
    b.mask = 0b1111'1111'1111'0000;
    b.moves = 0b1110'0010'1110'0000;
    solve(b, X, map);
    int n = bestMove(map, b, X, nullptr);
    assert(n == 14);

    b = (struct Board){
        0b1111'1100'0000'0111,
        0b0101'0000'0000'0111,
    };
    solve(b, O, map);
    n = bestMove(map, b, O, nullptr);
    assert(n==12);

    free(map);
}
