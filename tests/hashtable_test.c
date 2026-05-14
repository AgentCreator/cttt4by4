//
// Created by Oleksander Krainiak on 14.05.2026.
//
#include <assert.h>
#include "../lib.c"

int main(void) {
    struct Board b = {0, 0};
    place(&b, 0,X);
    place(&b, 1,X);
    struct Board qq = b;
    place(&b, 2,O);
    place(&b, 3,O);
    struct HashTable table = {};

    const auto q = (struct BotState){
        .move = X,
        .board_repr = b,
        .result = X};
    const auto q2 = (struct BotState){
        .move = X,
        .board_repr = qq,
        .result = X};
    struct BotState* ptr = table_find(&table, q);
    struct BotState* ptr2 = table_find(&table, q2);
    assert(ptr == nullptr);
    assert(ptr2 == nullptr);
    table_put(&table, q);
    table_put(&table, q2);
    ptr = table_find(&table, q);
    ptr2 = table_find(&table, q2);
    assert(ptr != nullptr);
    assert(ptr2 != nullptr);
    display(ptr->board_repr);
    display(ptr2->board_repr);

    table_free(&table);
}