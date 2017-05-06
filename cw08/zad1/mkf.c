#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void help() {
    printf("usage: mkf output_file");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        help();
        return 0;
    }

    srand((unsigned int) time(NULL));

    char *filename = argv[1];
    FILE *file = fopen(filename, "w");
    FILE *filer = fopen("input.txt", "r");
    fseek(filer, 0, SEEK_END);
    long length = ftell(filer)  - 1020;
    for (int i = 0; i < 1000; i++) {
        long offset = rand() % length;
        record_t record;
        record.id = i;
        fseek(filer, offset, SEEK_SET);
        fread(&record.text, sizeof(char), 1020, filer);
        fwrite(&record, sizeof(record), 1, file);
    }
    fclose(file);
    fclose(filer);
}
