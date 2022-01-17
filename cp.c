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
#include <errno.h>
#include <utime.h>

void cp_help() {
    printf("-f, --force : if  an existing destination file cannot be opened, remove it and try again (this option is ignored when the -n option is also used)\n");
    printf("-h, --help : \n");
    printf("-i, --interactive : prompt before overwrite (overrides a previous -n option)\n");
    printf("-p, --preserve : preserve the specified attributes (default:  mode,ownership,timestamps),  if  possible  additional  attributes: context, links, xattr, all\n");
    printf("-v, --verbose : explain what is being done\n");
}

void cp_verbose(char * src, char * dst, int happened) {
    if (!happened)
        printf("Nothing happened :(\n");
    else
        printf("'%s'->'%s'\n", src, dst);
}

int cp_interactive(char * dst) {
    printf("Are you sure?\n");
    char sure;
    scanf("%c", &sure);
    if (sure == 'n')
        return 0;
    if (sure != 'y') {
        printf("Wrong answer!\n");
        return 0;
    }

    return 1;
}

// void cp_force(char * src, char * dst) {
//     FILE * f1 = fopen(src, "r");
//     FILE * f2 = fopen(dst, "w");

//     if (errno == EACCES) {
//         remove(src);
//         f2 = fopen(src, "w");
//     }

//     fclose(f1);
//     fclose(f2);
// }

void cp_preserve(char * src, char * dst) {
    struct stat buf;
    lstat(src, &buf);
    chmod(dst, buf.st_mode);
    chown(dst, buf.st_uid, buf.st_gid);

    struct utimbuf time;
    time.actime = buf.st_atim.tv_sec;
    time.modtime = buf.st_mtim.tv_sec;
    utime(dst, &time);
}


void cp_default(char * src, char * dst, int is_h, int is_v, int is_i, int is_f, int is_p) {

    if (is_h)
        cp_help();

    FILE * f1 = fopen(src, "r");
    FILE * f2 = fopen(dst, "w");

    int happened = !(errno == EACCES);
    if (is_f) {
        if (!happened) {
            remove(src);
            f2 = fopen(src, "w");
        }
        happened = 1;
    }

    if (happened) {
        if (is_i) {
            happened = cp_interactive(dst);
        }

        void * buff = malloc(1000);
        while (fgets(buff, 100, f1) != NULL) {
            fwrite(buff, sizeof(char), 100, f2);
        }
        fclose(f1);
        fclose(f2);
        free(buff);

        if (is_p)
            cp_preserve(src, dst);
    }

    if (is_v)
        cp_verbose(src, dst, happened);
}

int main(int argc, char **argv)
{
    const struct option long_option[] =
    {
        {"help", no_argument, NULL, 'h'},
        {"verbose", no_argument, NULL, 'v'},
        {"interactive", no_argument, NULL, 'i'},
        {"force", no_argument, NULL, 'f'},
        {"preserve", no_argument, NULL, 'p'},
        {NULL, no_argument, NULL, 0}
    };
    int long_index;
    int opt;
    const char short_option[] = "hvifp";
    int flag_size = 5;

    int flag[256] = {0};


    while ((opt = getopt_long(argc, argv, short_option, long_option, &long_index)) != -1) {
        flag[opt] = 1;
    }

    cp_default(argv[optind], argv[optind + 1], flag['h'], flag['v'], flag['i'], flag['f'], flag['p']);
    return 0;
}
