#include "lib.h"

int flag[256];


void
unset(char *name)
{ 
    int r;

    if((r = syscall_envar(name, "", 3, 1)) < 0){
		if (r == -13)
			fwritef(1, "\033[0m\033[1;31mEnvironment var %s Not Exists!\n\033[0m", name);
		else if (r == -14)
			fwritef(1, "\033[0m\033[1;31mYou cannot unset a read-only variable!\n\033[0m");
        return;
    }
}

void
usage(void)
{ 
        fwritef(1, "usage: unset [vars...]\n");
        exit();
} 

void
umain(int argc, char **argv)
{  
    int i;
    if (argc == 0){
        return;    
    }
    else {
        for (i=1; i<argc; i++)
            unset(argv[i]);
    }
}
