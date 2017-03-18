#define _GNU_SOURCE

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>

void help() {
    printf("usage:\tzad3 <file name>");
}

void repl_help() {
    printf("lrXXXX - read-lock given byte XXXX of file\n"
                   "lwXXXX - write-lock given byte XXXX of file\n"
                   "lr'XXXX - read-lock given byte XXXX of file, waits for existing locks to release\n"
                   "lw'XXXX - write-lock given byte XXXX of file, waits for existing locks to release\n"
                   "ls - list all locks in file\n"
                   "rXXXX - read given byte XXXX of file\n"
                   "wXXXX,YYYY - write YYYY to given byte XXXX of file\n"
                   "q - quit\n"
                   "\n"
                   "number of digits in adresses doesn't matter, all numbers are parsed as hexadecimal\n"
                   "\n");
}

bool repl_lr(char *cmd, size_t cmd_len, int fd) {
    if (cmd_len < 2 || cmd[0] != 'l' || cmd[1] != 'r') return false;

    cmd += 2;

    bool wait = false;
    if (cmd[0] == '\'') {
        wait = true;
        cmd++;
    }

    char *rest;
    size_t offset = strtoul(cmd, &rest, 16);

    if (rest == cmd) {
        printf("no address specified!\n");
        return true;
    }

    struct flock fl;
    fl.l_type = F_RDLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = offset;
    fl.l_len = 1;

    if (fcntl(fd, wait ? F_SETLKW : F_SETLK, &fl) == -1) {
        perror("fcntl failed!");
        return true;
    }

    return true;
}

bool repl_lw(char *cmd, size_t cmd_len, int fd) {
    if (cmd_len < 2 || cmd[0] != 'l' || cmd[1] != 'w') return false;

    cmd += 2;

    bool wait = false;
    if (cmd[0] == '\'') {
        wait = true;
        cmd++;
    }

    char *rest;
    size_t offset = strtoul(cmd, &rest, 16);

    if (rest == cmd) {
        printf("no address specified!\n");
        return true;
    }

    struct flock fl;
    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = offset;
    fl.l_len = 1;

    if (fcntl(fd, wait ? F_SETLKW : F_SETLK, &fl) == -1) {
        perror("fcntl failed!");
        return true;
    }

    return true;
}

bool repl_ls(const char *cmd, size_t cmd_len, int fd) {
    if (strcmp(cmd, "ls") != 0) return false;

    off_t file_size;
    if ((file_size = lseek(fd, 0, SEEK_END)) == -1) {
        perror("cannot determine file size!");
        return true;
    }

    for (off_t i = 0; i < file_size; ++i) {
        struct flock fl;
        fl.l_type = F_WRLCK;
        fl.l_whence = SEEK_SET;
        fl.l_start = i;
        fl.l_len = 1;

        if (fcntl(fd, F_GETLK, &fl) == -1) {
            perror("fcntl failed!");
            return true;
        }

        if (fl.l_type != F_UNLCK) {
            char *lock_str;
            if (fl.l_type == F_EXLCK) {
                lock_str = "exclusive";
            } else if (fl.l_type == F_RDLCK) {
                lock_str = "read";
            } else if (fl.l_type == F_WRLCK) {
                lock_str = "write";
            } else {
                lock_str = "unknown";
            }

            printf("%-6X %-6d %s\n", (unsigned int) i, fl.l_pid, lock_str);
        }
    }

    return true;
}

bool repl_r(const char *cmd, size_t cmd_len, int fd) {
    if (cmd_len < 1 || cmd[0] != 'r') return false;

    char *rest;
    size_t offset = strtoul(cmd + 1, &rest, 16);

    if (rest == cmd + 1) {
        printf("no address specified!\n");
        return true;
    }

    if (lseek(fd, offset, SEEK_SET) == -1) {
        perror("couldn't seek file cursor!");
        return true;
    }

    void *buffer[1];
    if (read(fd, buffer, 1) != 1) {
        perror("couldn't read byte!");
        return true;
    }

    int b = ((char *) buffer)[0];
    printf("%x", b);
    if (isprint(b)) {
        printf(" = %c", (char) b);
    }
    printf("\n");

    return true;
}

bool repl_w(const char *cmd, size_t cmd_len, int fd) {
    if (cmd_len < 1 || cmd[0] != 'w') return false;

    char *rest;
    size_t offset = strtoul(cmd + 1, &rest, 16);

    if (rest == cmd + 1) {
        printf("no address specified!\n");
        return true;
    }

    size_t b;
    if (*rest == ',') {
        char *rest2;
        b = strtoul(rest + 1, &rest2, 16);

        if (rest2 == rest + 1) {
            printf("no byte specified!\n");
            return true;
        }
    } else {
        printf("but where is comma?");
        return true;
    }

    if (lseek(fd, offset, SEEK_SET) == -1) {
        perror("couldn't seek file cursor!");
        return true;
    }

    if (write(fd, &b, 1) == -1) {
        perror("couldn't write byte!");
        return true;
    }

    return true;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        help();
        return 0;
    }

    const char *file_name = argv[1];

    repl_help();

    char *cmd = calloc(64, sizeof(char));
    size_t cmd_buffer_len = 64 * sizeof(char);

    int fd = open(file_name, O_RDWR);
    if (fd == -1) {
        perror("cannot open file!");
        abort();
    }

    while (true) {
        printf("(zad3) ");
        fflush(stdout);

        ssize_t cmd_len;
        if ((cmd_len = getline(&cmd, &cmd_buffer_len, stdin)) == -1) {
            break;
        }

        if (cmd_len == 0) {
            continue;
        }

        if (cmd[cmd_len - 1] == '\n') {
            cmd[cmd_len - 1] = '\0';
            cmd_len--;
        }

        if (cmd_len == 1 && cmd[0] == 'q') {
            break;
        }

#define CMD(FN) if((FN)(cmd, cmd_len, fd)) continue;

        CMD(repl_lr);
        CMD(repl_lw);
        CMD(repl_ls);
        CMD(repl_r);
        CMD(repl_w);
    }

    close(fd);
    free(cmd);

    return 0;
}
