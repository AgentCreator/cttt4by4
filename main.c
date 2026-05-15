#include <assert.h>

#include "lib.c"
#include <stdlib.h>
#include <string.h>
#include <time.h>

void helptext(void) {
    printf("triple t cli\n");
    printf("q - quit\n");
    printf("h - help\n");
    printf("b - show board\n");
    printf("r - reset board\n");
    printf("s<X|O> - starts a new game, with the bot playing the given player\n");
    printf("<num> - place an X/O at <num>\n");
}

bool print_result(const struct Board board) {
    const int r = result(board);
    if (!r) return false;
    if (r == X) printf("X won!\n");
    else if (r == O) printf("O won!\n");
    else printf("draw!\n");
    return true;
}

void assign_players(const char *command, enum Player *bot) {
    char e;
    sscanf(command, "s%c", &e);
    if (e == 'X' || e == 'x') *bot = X;
    else if (e == 'O' || e == 'o') *bot = O;
}


int main(void) {
    struct timespec start, end;
    struct Board b = {0, 0};
    struct HashTable *map = calloc(1, sizeof(struct HashTable));
    printf("solving...\n");
    fflush(stdout);
    timespec_get(&start, TIME_UTC);
    solve(b, X, map);
    timespec_get(&end, TIME_UTC);
    printf("solved (in %fs)\n", (double)end.tv_sec - (double)start.tv_sec + (double)(end.tv_nsec - start.tv_nsec) * 1e-9);
    helptext();
    enum Player curPlayer = 0;
    enum Player bot = 0;
    while (true) {
        char input[5];
        printf("%c>", charOf(curPlayer));
        fflush(stdout);

        scanf("%4s", input);
        // printf("%d", qq);
        if (input[0] == 'q') break;
        if (input[0] == 'h') helptext();
        else if (input[0] == 'r') {
            b = (struct Board){0, 0};
            bot = 0;
            curPlayer = 0;
        } else if (input[0] == 's') {
            assign_players(input, &bot);
            curPlayer = X;
            printf("outcome: 0\n");
            if (bot == X) {
                const int best_move = bestMove(map, b, curPlayer, nullptr);
                place(&b, best_move, curPlayer);
                curPlayer *= -1;
            }
            display(b);
        } else if (input[0] == 'b') display(b);
        else {
            if (result(b)) {
                printf("restart game to make a move!\n");
                continue;
            }
            char *left;
            long i = strtol(input, &left, 10);
            if (left == input) continue;
            if (get(b, i)) continue;
            place(&b, i, curPlayer);
            if (result(b)) goto res;
            curPlayer *= -1;
            int outcome;
            const int best_move = bestMove(map, b, curPlayer, &outcome);
            place(&b, best_move, curPlayer);

            curPlayer *= -1;
            printf("outcome: %d\n", outcome);
            res:
            display(b);
            print_result(b);
        }
    }
    table_free(map);
    free(map);
    // printf("%d", q.result);
}
