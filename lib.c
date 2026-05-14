//
// Created by Oleksander Krainiak on 14.05.2026.
//
#ifndef LIB_FILE
#define LIB_FILE

#include <stdio.h>
#include <stdlib.h>


#define DRAW 2

enum Player: signed char { X = 1, O = -1 };

enum Player BOT = O;

char charOf(const int p) {
    if (p == 0) return '.';
    if (p == X) return 'X';
    if (p == O) return 'O';
    __builtin_unreachable();
}

struct Board {
    unsigned short mask, moves;
};

unsigned to_int(struct Board b) {
    return (((unsigned)b.mask)<<16)+b.moves;
}

struct Board from_int(unsigned b) {
    return (struct Board){
    .mask = b >> 16,
    .moves = b & 0xffff};
}

int get(const struct Board b, const unsigned short at)  {
    if ((b.mask >> (15 - at) & 1) == 0) return 0;
    return b.moves >> (15 - at) & 1 ? X : O;
}

void place(struct Board *b, const unsigned short at, const enum Player put) {
    b->mask |= 1 << (15 - at);
    b->moves |= (put == X) << (15 - at);
}

/**
 *
 * @param b the board
 * @return 0 if game is still going, 1 if game is won by X, -1 if game is won by O, 2 if draw
 */
int result(const struct Board b) {
    constexpr unsigned short possible_ends[] = {
        0xf000,
        0x0f00,
        0x00f0,
        0x000f,

        0b1000'1000'1000'1000,
        0b0100'0100'0100'0100,
        0b0010'0010'0010'0010,
        0b0001'0001'0001'0001,

        0b1000'0100'0010'0001,
        0b0001'0010'0100'1000,
    };
    constexpr int size = sizeof possible_ends / sizeof possible_ends[0];
    for (int i = 0; i < size; ++i) {
        const unsigned short k = possible_ends[i];
        if ((b.mask & k) != k) continue;
        const unsigned short r = b.moves & k;
        if (r == k) return X;
        if (r == 0) return O;
    }
    if (~b.mask & 0xffff) return 0;
    return DRAW;
}

void display(const struct Board b) {
    char result[] =
            ". | . | . | .\n"
            "--+---+---+--\n"
            ". | . | . | .\n"
            "--+---+---+--\n"
            ". | . | . | .\n"
            "--+---+---+--\n"
            ". | . | . | .\n";
    constexpr int row_size = sizeof ". | . | . | .\n" - 1;

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            constexpr int indices[] = {0, 4, 8, 12};
            const int q = get(b, i * 4 + j);
            result[row_size * 2 * i + indices[j]] = charOf(q);
        }
    }
    printf("%s", result);
}

struct BotState {
    enum Player move;
    struct Board board_repr;
    signed char result; // X, O, or DRAW
};

bool bot_state_eql(struct BotState a, struct BotState b) {
    return a.move == b.move && a.result == b.result && to_int(a.board_repr) == to_int(b.board_repr);
}

size_t hash(struct BotState s) {
    return (to_int(s.board_repr)<<4)+((s.move+1)<<2) + (s.result+1);
}

struct Slice {
    size_t size;
    struct BotState *s __attribute__((counted_by(size)));
};

struct HashTable {
    struct Slice table[1<<15];
};

void table_put(struct HashTable* h, const struct BotState s) {
    size_t hashed = hash(s);
    struct Slice* sl = &h->table[hashed % ((1<<15)-1)];
    struct BotState* new_s = realloc(sl->s, ++sl->size*sizeof(struct BotState));
    if (!new_s) {
        printf("realloc failed\n");
        exit(-1);
    }
    new_s[sl->size-1] = s;
    sl->s = new_s;
}

void table_free(const struct HashTable* h) {
    for (int i = 0; i < 1<<15; ++i) {
        free(h->table[i].s);
    }
}

struct BotState* table_find(const struct HashTable* h, const struct BotState s) {
    const size_t hashed = hash(s);
    const struct Slice* sl = &h->table[hashed % ((1<<15)-1)];
    for (size_t i = 0; i < sl->size; ++i) {
        struct BotState* q = &sl->s[i];
        if (bot_state_eql(*q, s)) return q;
    }
    return nullptr;
}


int solve(struct Board b, enum Player p);

#endif
