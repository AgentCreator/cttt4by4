#include <stdio.h>


enum Player { X = 1, O = -1 };

char charOf(const int p) {
    if (p == 0) return '.';
    if (p == X) return 'X';
    if (p == O) return 'O';
    __builtin_unreachable();
}

struct Board {
    unsigned short mask, moves;
};


int get(const struct Board b, const unsigned short at) {
    if ((b.mask >> (15-at) & 1) == 0) return 0;
    return b.moves >> (15-at) & 1 ? X : O;
}

void place(struct Board *b, const unsigned short at, const enum Player put) {
    b->mask |= 1 << (15-at);
    b->moves |= (put==X) << (15-at);
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
            const int q = get(b, i*4+j);
            result[row_size*2*i+indices[j]] = charOf(q);
        }
    }
    printf("%s", result);
}

int main(void) {
    struct Board b = {0, 0};
    place(&b, 0, X);
    place(&b, 1, X);
    place(&b, 2, X);
    place(&b, 3, X);
    place(&b, 4, O);
    display(b);
}
