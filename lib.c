//
// Created by Oleksander Krainiak on 14.05.2026.
//

#include <stdio.h>
#include <stdlib.h>



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
    enum Player move;
    struct Board board_repr;
    signed char result; // X (1), O (-1), or DRAW (0, in this case)
};

bool bot_state_eql(struct BotState a, struct BotState b) {
    return a.move == b.move && to_int(a.board_repr) == to_int(b.board_repr);
}

#define KNUTH_CONSTANT 0x9E3779B1
size_t hash(struct BotState s) {
    unsigned long res = to_int(s.board_repr);
    // everybody was using xor here https://stackoverflow.com/questions/13389631/whats-a-good-hash-function-for-struct-with-3-unsigned-chars-and-an-int-for-uno
    // so i decided to use it too
    res ^= s.move;
    return res * KNUTH_CONSTANT; // apparently there is a constant that makes all hashes equally distributed, cool
}

struct Slice {
    size_t size;
    size_t capacity;
    struct BotState *s __attribute__((counted_by(capacity)));
};

struct HashTable {
    struct Slice table[1 << 16];
};

void table_put(struct HashTable *h, const struct BotState s) {
    size_t hashed = hash(s);
    struct Slice *sl = &h->table[hashed % ((1 << 16) - 1)];
    // size_t prev_size = sl->size;
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
    for (int i = 0; i < 1 << 16; ++i) {
        free(h->table[i].s);
    }
}

struct BotState *table_find(const struct HashTable *h, const struct BotState s) {
    const size_t hashed = hash(s);
    const struct Slice *sl = &h->table[hashed % ((1 << 16) - 1)];
    for (size_t i = 0; i < sl->size; ++i) {
        struct BotState *q = &sl->s[i];
        if (bot_state_eql(*q, s)) return q;
    }
    return nullptr;
}


struct BotState solve(const struct Board b, enum Player p, struct HashTable *cache, int depth) {
    struct BotState res = {
        .move = p,
        .board_repr = b,
        .result = (signed char)(-p*2) // garbage
    };
    struct BotState *q;
    if ((q = table_find(cache, res))) {
        return *q;
    }
    int r;
    if ((r = result(b))) {
        if (r == DRAW) r = 0;
        res.result = (signed char)r;
        return res;
    }

    bool is_init = false;
    struct BotState best_move = {};
    for (int i = 0; i < 16; ++i) {
        if (get(b, i)) continue;
        struct Board new_board = b;
        place(&new_board, i, p);
        const struct BotState qq = solve(new_board, -p, cache, depth + 1);
        if (!is_init) {
            best_move = qq;
            is_init = true;
        } else {
            if ((p == X && qq.result > best_move.result) ||
                (p == O && qq.result < best_move.result)) {
                if (best_move.result == p) {
                    break;
                }
                best_move = qq;
            }
        }
    }
    res.result = best_move.result;
    table_put(cache, res);
    return res;
}
