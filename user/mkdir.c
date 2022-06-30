#include "lib.h"

int flag[256];


void
mkdir(char *path, char *prefix)
{ 
    int r, fd;
    struct Stat st;
    char curpath[MAXPATHLEN] = {'\0'};

    if ((r = curpath_get(curpath)) < 0) {
        fwritef(1, "mkdir: cannot get environment var [curpath]\n");
    }
    if(path[0] == '/')
    {
        strcpy(curpath, path);
    }
    else
    {
        if(curpath[strlen(curpath) - 1] != '/')
            strcat(curpath, "/");
        strcat(curpath, path);
    }

    if((r = create(curpath, FTYPE_DIR)) < 0){
        fwritef(1, "\033[0m\033[1;31mDirectory %s Already Exists!\033[0m\n", curpath);
        return;
    }
    fwritef(1, "\033[0m\033[1;33mCreated Directory %s successfully!\033[0m\n", curpath);
}

void
usage(void)
{
        fwritef(1, "usage: mkdir [-dFl] [file...]\n");
        exit();
} 

void
umain(int argc, char **argv)
{  
    int i;
    ARGBEGIN{
    default:
            usage();
    case 'd':
    case 'F':
    case 'l':
            flag[(u_char)ARGC()]++;
            break;
    }ARGEND

    if (argc == 0){
        return;    
    }
    else {
        for (i=0; i<argc; i++)
                    mkdir(argv[i], argv[i]);
    }
}
