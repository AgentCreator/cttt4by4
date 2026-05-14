//
// Created by Oleksander Krainiak on 14.05.2026.
//
#include <assert.h>
#include "../lib.c"

int main() {
    struct Board b = {};
    place(&b, 0, X);
    place(&b, 2, O);
    place(&b, 3, O);
    assert(get(b, 0) == X);
    assert(get(b, 2) == O);
    assert(get(b, 1) == 0);
    assert(result(b) == 0);
    place(&b, 1, X);
    place(&b, 4, O);
    place(&b, 5, O);
    place(&b, 6, X);
    place(&b, 7, X);
    place(&b, 8, X);
    place(&b, 9, X);
    place(&b, 10, O);
    place(&b, 11, O);
    place(&b, 12, O);
    place(&b, 13, O);
    place(&b, 14, X);
    place(&b, 15, X);
    assert(result(b) == DRAW);
    struct Board b2 = {};
    place(&b2, 5, X);
    place(&b2, 10, X);
    place(&b2, 0, X);
    place(&b2, 15, X);
    b.moves |= b2.moves;
    assert(result(b) == X);
    b.moves ^= b2.moves;
    assert(result(b) == O);

}