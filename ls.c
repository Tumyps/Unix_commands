#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>


void ls_long(struct dirent ** drt, int is_a) {
    struct stat buf;
    if (!is_a && (*drt)->d_name[0] == '.')
        return;

    lstat((*drt)->d_name, &buf);

    char * s = ctime(&(buf.st_mtime));

    struct passwd * psw = getpwuid(buf.st_uid);
    struct group * grp = getgrgid(buf.st_gid);

    printf("%ld %s %s %10ld ", buf.st_nlink, psw->pw_name, grp->gr_name, buf.st_size);

    for (int i = 4; i < 16; i++)
        printf("%c", s[i]);
    printf(" %s\n", (*drt)->d_name);
}

void  ls_default(char * name, int is_a, int is_l) {
    DIR * dir = opendir(name);
    struct dirent * drt;
    struct stat buf;

    while ((drt = readdir(dir)) != NULL) {
        if (!is_a && drt->d_name[0] == '.')
            continue;

        if (is_l)
            ls_long(&drt, is_a);
        else
            printf("%s ", drt->d_name);
    }

    closedir(dir);
    if (!is_l)
        printf("\n");
}

void ls_recursive(char * name, int is_a, int is_l) {
    printf("%s:\n", name);

    DIR * dir = opendir(name);
    struct dirent * drt;
    struct stat buf;
    char is_dir[100][1000];
    int id = 0;

    while ((drt = readdir(dir)) != NULL) {
        if (!is_a && drt->d_name[0] == '.')
            continue;

        if (is_l)
            ls_long(&drt, is_a);
        else
            printf("%s ", drt->d_name);

        lstat(drt->d_name, &buf);
        if (drt->d_name[0] != '.' && drt->d_type == 4) {
            strcpy(is_dir[id], name);
            strcat(is_dir[id], "/");
            strcat(is_dir[id], drt->d_name);
            ++id;
        }
    }
    //closedir(dir);

    printf("\n\n");
    for (int i = 0; i < id; ++i) {
        ls_recursive(is_dir[i], is_a, is_l);
    }
    closedir(dir);
}

int main(int argc, char **argv)
{
    const struct option long_option[] =
    {
        {"all", no_argument, NULL, 'a'},
        {"long", no_argument, NULL, 'l'},
        {"recursive", no_argument, NULL, 'R'},
        {NULL, no_argument, NULL, 0}
    };
    int long_index;
    int opt;
    const char short_option[] = "alR";
    int flag_size = 5;

    int flag[256] = {0};

    while ((opt = getopt_long(argc, argv, short_option, long_option, &long_index)) != -1)
    {
        flag[opt] = 1;
    }

    int no_flag = 1;
    for (int i = 0; i < flag_size; ++i) {
        if (flag[short_option[i]])
            no_flag = 0;
    }
    char * name;
    if (argv[optind] == NULL)
        name = ".";
    else
        name = argv[optind];


    if (flag['R'])
        ls_recursive(name, flag['a'], flag['l']);
    else
        ls_default(name, flag['a'], flag['l']);

    return 0;
}