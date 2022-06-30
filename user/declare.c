#include "lib.h"

void set(char *name, char *value, int op, int isEnvar) { 
    int r;
    if((r = syscall_envar(name, value, 2, isEnvar)) < 0){
        if (r == -13) {
			syscall_envar(name, value, 0, isEnvar);
			if (op == 5) {
				syscall_envar(name, value, 5, isEnvar);	
			}
		} else if (r == -14) {
			fwritef(1, "\033[0m\033[1;31mFail to declare %s because it is read-only.\033[0m\n", name);	
		}
        return;
    }
}

void
usage(void)
{ 
        fwritef(1, "usage: declare -xr [var [=value]]\n");
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
		if (argc == 1) {
			// declare
			syscall_envar("", "", 4, 0);
		}
		else if (argc == 2) {
			// declare NAME=VALUE or declare NAME
			// regular partly envar
			int flag_name = -1;
			int name_len = 0, value_len = 0;
			char name[64], value[256];
			for (i = 0; i < strlen(argv[1]); i++) {
				if (argv[1][i] == '=' && flag_name == -1) {
					flag_name = 1;
					continue;	
				}	
				if (flag_name == -1) {
					name[name_len++] = argv[1][i];	
				} else {
					value[value_len++] = argv[1][i];	
				}
			}
			name[name_len] = '\0';
			value[value_len] = '\0';
			//strcpy(testname, name);
			//strcpy(testvalue, value);
			set(name, value, 2, 0);
		}
		else if (argc == 3) {
			//declare -xr NAME=VALUE or declare -xr NAME
			int flag_x = -1, flag_r = -1,  flag_name = -1;
			int name_len = 0, value_len = 0;
			char name[64], value[256];
			for (i = 0; i < strlen(argv[2]); i++) {
				if (argv[2][i] == '=' && flag_name == -1) {
					flag_name = 1;
					continue;	
				}
				if (flag_name == -1) {
					name[name_len++] = argv[2][i];
				} else {
					value[value_len++] = argv[2][i];	
				}
			}
			name[name_len] = '\0';
			value[value_len] = '\0';
			for (i = 0; i < strlen(argv[1]); i++) {
				if (argv[1][i] == 'x') {
					flag_x = 1;	
				}	
				if (argv[1][i] == 'r') {
					flag_r = 1;	
				}
			}	
			if (flag_x == 1 && flag_r == 1) {
				// read-only envar
				set(name, value, 5, 1);
			}	
			else if (flag_x == 1) {
				// regular envar
				set(name, value, 2, 1);
			}
			else if (flag_r == 1) {
				// read-only partly envar
				set(name, value, 5, 0);
			}
		}
    }
}
