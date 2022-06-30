#include "lib.h"

int flag[256];

void walk_dir(char *path, int level)
{
    int fd, n;
    struct File f;
    char nextDir[MAXPATHLEN] = {0};
    
    if ((fd = open(path, O_RDONLY)) < 0)
        user_panic("Fail to open %s: %e", path, fd);
    while ((n = readn(fd, &f, sizeof f)) == sizeof f) {
        if (f.f_name[0]) {
            tree_print(f.f_name, level, f.f_type);

            if(f.f_type == FTYPE_DIR) {
                strcpy(nextDir, path);
                strcat(nextDir, "/");
                strcat(nextDir, f.f_name);
                walk_dir(nextDir, level + 1);
            }
        }
    }
}

void tree_print(char *fname, int level, int ftype) {
	int i;
	if (level > 0) {
		if (ftype == FTYPE_DIR) {
			fwritef(1, "\033[0m\033[1;32m|\033[0m");
		} else {
			fwritef(1, "\033[0m\033[1;34m|\033[0m");	
		}
	}
	for (i = 0; i < level; i++) {
		fwritef(1, "	");	
	}
	if (ftype == FTYPE_DIR) {
		fwritef(1, "\033[0m\033[1;32m|----%s\033[0m\n", fname);
	} else {
		fwritef(1, "\033[0m\033[1;34m|----%s\033[0m\n", fname);
	}
}



    void
tree_start(char *path, u_int recursive)
{ 
    fwritef(1, ". %s\n", path);
    walk_dir(path, 0);
    return;
}

    void
tree(char *path, char *prefix)
{ 
    int r, fd;
    struct Stat st;
    if ((r=stat(path, &st)) < 0)
        user_panic("stat %s: %e", path, r);
    if (st.st_isdir && !flag['d'])
    {
        tree_start(path, -1);
    }
    else
    {
        tree_start(path, -1);
        //ls1(0, st.st_isdir, st.st_size, path);
    }
}


    void
usage(void)
{
    fwritef(1, "usage: tree [-dFl] [file...]\n");
    exit();
}

    void
umain(int argc, char **argv)
{
    int i;
    char curpath[MAXPATHLEN];
    ARGBEGIN{
        default:
            usage();
        case 'd':
        case 'F':
        case 'l':
            flag[(u_char)ARGC()]++;
            break;
    }ARGEND
    //syscall_curpath(curpath, 0);
    if (argc == 0){
        //tree(curpath, "");
        tree("/", "");
    }
    else {
        for (i=0; i<argc; i++)
            tree(argv[i], argv[i]);
    }

}
