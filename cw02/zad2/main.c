#define _XOPEN_SOURCE 500
#include <ftw.h>
#include <dirent.h>

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <errno.h>

void help() {
    printf("usage:\tzad2 <stat|nftw> <directory> <max size>\n");
}

void show_info(const char *fpath, const struct stat *sb) {
    char flag_str[10];
    flag_str[0] = (char) ((S_ISDIR(sb->st_mode)) ? 'd' : '-');
    flag_str[1] = (char) ((sb->st_mode & S_IRUSR) ? 'r' : '-');
    flag_str[2] = (char) ((sb->st_mode & S_IWUSR) ? 'w' : '-');
    flag_str[3] = (char) ((sb->st_mode & S_IXUSR) ? 'x' : '-');
    flag_str[4] = (char) ((sb->st_mode & S_IRGRP) ? 'r' : '-');
    flag_str[5] = (char) ((sb->st_mode & S_IWGRP) ? 'w' : '-');
    flag_str[6] = (char) ((sb->st_mode & S_IXGRP) ? 'x' : '-');
    flag_str[7] = (char) ((sb->st_mode & S_IROTH) ? 'r' : '-');
    flag_str[8] = (char) ((sb->st_mode & S_IWOTH) ? 'w' : '-');
    flag_str[9] = (char) ((sb->st_mode & S_IXOTH) ? 'x' : '-');
    flag_str[10] = '\0';
    printf("%-50s %7jd %-3s %s", fpath, (intmax_t) sb->st_size, flag_str, ctime(&sb->st_mtime));
}

void do_stat(const char *path, size_t max_size) {
    char rp[PATH_MAX];
    if (realpath(path, rp) == NULL) {
        printf("failed to determine real path of %s\n", path);
        return;
    }

    struct stat sb;
    if (lstat(rp, &sb) == -1) {
        printf("failed to get stat for %s, errno: %d\n", rp, errno);
        return;
    }

    if (S_ISDIR(sb.st_mode)) {
        DIR *dir = opendir(rp);
        if (dir == NULL) {
            printf("failed to open %s\n", rp);
            return;
        }

        struct dirent *ds;
        while ((ds = readdir(dir)) != NULL) {
            if (strcmp(ds->d_name, ".") == 0) continue;
            if (strcmp(ds->d_name, "..") == 0) continue;

            char child_file_name[PATH_MAX];
            snprintf(child_file_name, PATH_MAX, "%s/%s", rp, ds->d_name);

            do_stat(child_file_name, max_size);
        }

        closedir(dir);
    } else if (S_ISREG(sb.st_mode) && (intmax_t) sb.st_size <= max_size) {
        show_info(rp, &sb);
    }
}

size_t nftw_visitor_max_size = 0;

int nftw_visitor(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf) {
    if ((intmax_t) sb->st_size <= nftw_visitor_max_size) {
        show_info(fpath, sb);
    }

    return 0;
}

void do_nftw(const char *path, size_t max_size) {
    nftw_visitor_max_size = max_size;
    if (nftw(path, nftw_visitor, 16, FTW_PHYS) == -1) {
        printf("nftw failed!\n");
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        help();
        return 0;
    }

    const char *mode = argv[1];
    const char *path = argv[2];
    size_t max_size = (size_t) atoi(argv[3]);

    if (strcmp(mode, "stat") == 0) {
        do_stat(path, max_size);
    } else if (strcmp(mode, "nftw") == 0) {
        do_nftw(path, max_size);
    } else {
        help();
    }

    return 0;
}
