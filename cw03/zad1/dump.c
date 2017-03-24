#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("usage: zad1_dump <env name>\n");
        return 0;
    }

    const char *env = getenv(argv[1]);
    if (env == NULL) {
        fprintf(stderr, "couldn't find env var %s\n", argv[1]);
    } else {
        printf("%s\n", env);
    }

    return 0;
}
