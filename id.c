#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>
#include <stdlib.h>

void id_default() {
	int id = getuid();
	struct passwd * psw = getpwuid(id);
	char * my_name = psw->pw_name;
	printf("uid=%d(%s) ", id, my_name);

	int gid = getgid();
	struct group * grp = getgrgid(id);
	printf("gid=%d(%s) ", id, grp->gr_name);

	printf("groups=%d(%s)", id, grp->gr_name);

	struct group * gr;
	while ((gr = getgrent()) != NULL) {
		for (char ** name = gr->gr_mem; *name != NULL; *name++) {
			if (!strcmp(*name, my_name))
				printf(",%d(%s)", gr->gr_gid, gr->gr_name);
		}
	}
	printf("\n");
}

void id_g() {
	printf("%d\n", getuid());
}

void id_u() {
	printf("%d\n", getuid());
}


void id_G() {
	int id = getuid();
	struct passwd * psw = getpwuid(id);
	char * my_name = psw->pw_name;
	struct group * gr;
	while ((gr = getgrent()) != NULL) {
		for (char ** name = gr->gr_mem; *name != NULL; *name++) {
			if (!strcmp(*name, my_name))
				printf("%d ", gr->gr_gid);
		}
	}
	printf("\n");

}

int main(int argc, char ** argv) {
	int opt;
	const char short_option[] = "guG";


	int flag_g = 0, flag_u = 0, flag_G = 0;

	while ((opt = getopt(argc, argv, short_option)) != -1) {
		switch (opt) {
		case 'g':
			flag_g = 1;
			break;
		case 'u':
			flag_u = 1;
			break;
		case 'G':
			flag_G = 1;
			break;
		default:
			printf("Wrong flag!\n");
			break;
		}
	}

	int no_flag = !flag_G && !flag_g && !flag_u;

	if (no_flag)
		id_default();
	if (flag_g)
		id_g();
	if (flag_u)
		id_u();
	if (flag_G)
		id_G();
}