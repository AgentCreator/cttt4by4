//
// Created by Oleksander Krainiak on 14.05.2026.
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define DRAW 2

enum Player: signed char { X = 1, O = -1 };

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
    return ((unsigned) b.mask << 16) + b.moves; // NOLINT
}

struct Board from_int(unsigned b) {
    return (struct Board){
        .mask = b >> 16,
        .moves = b & 0xffff
    };
}

int get(const struct Board b, const unsigned short at) {
    if ((b.mask >> (15 - at) & 1) == 0) return 0;
    return b.moves >> (15 - at) & 1 ? X : O;
}

void place(struct Board *b, const unsigned short at, const enum Player put) {
    b->mask |= 1 << (15 - at);
    b->moves |= (put == X) << (15 - at);
}

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
    struct Board board_repr;
    signed char result; // X (1), O (-1), or DRAW (0, in this case)
};

bool bot_state_eql(struct BotState a, struct BotState b) {
    return to_int(a.board_repr) == to_int(b.board_repr);
}

#define KNUTH_CONSTANT 0x9E3779B1

size_t hash(struct BotState s) {
    unsigned long res = to_int(s.board_repr);
    return res * KNUTH_CONSTANT; // apparently there is a constant that makes all hashes equally distributed, cool
}

struct Slice {
    size_t size;
    size_t capacity;
    struct BotState *s __attribute__((counted_by(capacity)));
};

#define TABLE_SIZE 1<<20

struct HashTable {
    struct Slice table[TABLE_SIZE];
};

void table_put(struct HashTable *h, const struct BotState s) {
    size_t hashed = hash(s);
    struct Slice *sl = &h->table[hashed % ((TABLE_SIZE) - 1)];
    if (sl->size >= sl->capacity) {
        sl->capacity = sl->capacity == 0 ? 2 : sl->capacity * 2;
        struct BotState *new_s = realloc(sl->s, sl->capacity * sizeof(struct BotState));
        if (!new_s) {
            printf("realloc failed\n");
            exit(-1);
        }
        sl->s = new_s;
    }
    sl->s[sl->size] = s;
    sl->size++;
}

void table_free(const struct HashTable *h) {
    for (int i = 0; i < TABLE_SIZE; ++i) {
        free(h->table[i].s);
    }
}

struct BotState *table_find(const struct HashTable *h, const struct BotState s) {
    const size_t hashed = hash(s);
    const struct Slice *sl = &h->table[hashed % ((TABLE_SIZE) - 1)];
    for (size_t i = 0; i < sl->size; ++i) {
        struct BotState *q = &sl->s[i];
        if (bot_state_eql(*q, s)) return q;
    }
    return nullptr;
}

struct Board map_board(const struct Board inp, const int *transform) {
    struct Board res = {0, 0};
    for (int i = 0; i < 16; ++i) {
        unsigned mask_bit = inp.mask >> i & 1u;
        unsigned moves_bit = inp.moves >> i & 1u;
        res.mask |= mask_bit << (transform[i] - 1);
        res.moves |= moves_bit << (transform[i] - 1);
    }
    return res;
}

bool cache_lookup(const struct HashTable *h, const struct BotState s, struct BotState *out) {
    struct BotState s2 = s;
    struct BotState *res;
    constexpr int rot90[] = {
        4, 8, 12, 16,
        3, 7, 11, 15,
        2, 6, 10, 14,
        1, 5, 9, 13,
    };

    for (int i = 0; i < 4; ++i) {
        if ((res = table_find(h, s2))) {
            struct BotState new_s = s;
            new_s.result = res->result;
            *out = new_s;
            return true;
        }
        s2.board_repr = map_board(s2.board_repr, rot90);
    }
    out = nullptr;
    return false;
}
#undef CHECK

struct BotState solve(const struct Board b, enum Player p, struct HashTable *cache) {
    struct BotState res = {
        .board_repr = b,
        .result = (signed char) (-p * 2) // garbage
    };
    struct BotState q = {};
    if (cache_lookup(cache, res, &q)) {
        return q;
    }
    int r;
    if ((r = result(b))) {
        if (r == DRAW) r = 0;
        res.result = (signed char) r;
        table_put(cache, res);
        return res;
    }

    bool is_init = false;
    struct BotState best_move = {};
    for (int i = 0; i < 16; ++i) {
        if (get(b, i)) continue;
        struct Board new_board = b;
        place(&new_board, i, p);
        const struct BotState qq = solve(new_board, -p, cache);
        if (!is_init) {
            best_move = qq;
            is_init = true;
        } else {
            if ((p == X && qq.result > best_move.result) ||
                (p == O && qq.result < best_move.result)) {
                best_move = qq;
            }
        }
    }
    res.result = best_move.result;
    table_put(cache, res);
    return res;
}

int bestMove(const struct HashTable *map, const struct Board b, const enum Player curPlayer, int *outcome) {
    int bestMove = -1;
    int bestResult = -2 * curPlayer;
    for (int i = 0; i < 16; ++i) {
        if (get(b, i)) continue;
        struct Board new_b = b;
        place(&new_b, i, curPlayer);

        struct BotState w;
        assert(cache_lookup(map, (struct BotState){
        .board_repr = b}, &w));
        if ((curPlayer == X && w.result > bestResult) || (curPlayer == O && w.result < bestResult)) {
            bestResult = (int) w.result;
            bestMove = i;
        }
    }
    if (outcome) *outcome = bestResult;
    return bestMove;
}
