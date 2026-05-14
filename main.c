#include "lib.c"

void helptext(void) {
    printf("triple t cli\n");
    printf("q - quit\n");
    printf("h - help\n");
    printf("b - board\n");
    printf("<num> - place an X/O at <num>\n");
}

int main(void) {
    struct Board b = {0, 0};
    // display(b);
    struct HashTable map = {};
    printf("solving...\n");
    fflush(stdout);
    struct BotState q = solve(b, X, &map, 0);
    printf("solved\n");
    helptext();
    // printf("%d", q.result);
}