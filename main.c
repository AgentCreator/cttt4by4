#include <stdio.h>
#include <stdlib.h>

#define DRAW 2

enum Player { X = 1, O = -1 };

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

size_t to_size_t(struct Board b) {
    return (b.mask<<16)+b.moves;
}


/**
 *
 * @param b the board
 * @param at the index to take the value from, starting from top left.
 * @return 0 if empty, 1 if X, -1 if O
 */
int get(const struct Board b, const unsigned short at) {
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
int result(struct Board b) {
    constexpr unsigned short possible_ends[] = {
        0b1111'0000'0000'0000,
        0b0000'1111'0000'0000,
        0b0000'0000'1111'0000,
        0b0000'0000'0000'1111,

        0b1000'1000'1000'1000,
        0b0100'0100'0100'0100,
        0b0010'0010'0010'0010,
        0b0001'0001'0001'0001,

        0b1000'0100'0010'0001,
        0b0001'0010'0100'1000,
    };
    constexpr int size = sizeof possible_ends / sizeof possible_ends[0];
    for (int i = 0; i < size; ++i) {
        unsigned short k = possible_ends[i];
        if ((b.mask & k) != k) continue;
        unsigned short r = b.moves & k;
        if (r == k) return X;
        if (r == 0) return O;
    }
    if (~b.mask == 0) return DRAW;
    return 0;
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

// int get_quality(const struct Board b) {
//
// }

// unsigned char move_table[1ull << 32];
// unsigned char score_table[1ull << 32];

/**
 * @param state current board
 * @param move whose move it is
 * @param move_table
 * @param score_table
 * @return the score of the position
 */
int solve(struct Board state, enum Player move, signed char * _Nonnull move_table, signed char * _Nonnull score_table) {

    if (move_table[to_size_t(state)] != 0) {
        return score_table[to_size_t(state)];
    }
    int q;
    if ((q = result(state))) return q == DRAW ? 0 : q;

    int best_outcome = -move;
    int best_move = -1;

    for (int i = 0; i < 16; i++) {
        if (get(state, i) != 0) continue;
        struct Board new_board = state;
        place(&new_board, i, move);
        int r = solve(new_board, -move, move_table, score_table);
        if ((r == X && r > best_outcome) || (r == O && r < best_outcome)) {
            best_outcome = r;
            best_move = i;
        }
    }
    move_table[to_size_t(state)] = best_move;
    score_table[to_size_t(state)] = best_outcome;
    return best_outcome;
}

int main(void) {
    struct Board b = {0, 0};
    signed char * move_table = calloc(1ull<<32,1);
    signed char * score_table = calloc(1ull<<32,1);
    int r = solve(b, X, move_table, score_table);
    printf("%d\n", r);
    free(move_table);
    free(score_table);
}
