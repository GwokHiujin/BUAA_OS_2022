#include "lib.h"

const char *CURPATH = "curpath";

void curpath_init(char *path) {
    int r;
    if ((r = syscall_envar(CURPATH, path, 0, 1)) < 0)
		user_panic("Fail to initialize curpath: %d.", r);
}

int curpath_get(char *path) {
    int r;
    if ((r = syscall_envar(CURPATH, path, 1, 1)) < 0) 
		return r;
}

int curpath_set(char *path) {
    int r;
    if ((r = syscall_envar(CURPATH, path, 2, 1)) < 0) 
		return r;
}

int curpath_get_parent(char *path) {
    int r, i;
    if ((r = curpath_get(path)) < 0) return r;
    if (strlen(path) == 1) return 0;

    for (i = strlen(path) - 2; path[i - 1] != '/'; i--);
    path[i] = 0;
}
