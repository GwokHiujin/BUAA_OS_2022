#include "lib.h"

static int cnt;

void test_cnt() {
	cnt++;
	writef("%d\n", cnt);
}

void history_init() {
    int r;
    if((r = create("/.history", FTYPE_REG)) < 0)
		if (r != -11) 
			user_panic("Fail to initialize .history: %d.", r);
}

void history_store(char *s) {
    int r;
    if ((r = open("/.history", O_RDWR | O_CREAT | O_APPEND)) < 0)
		user_panic("Fail to open /.history.");
    fwritef(r, "%s\n", s); 
    close(r);
}

int history_load(char (*cmd)[128]) {
    int r, fd, cur = 1;
    char buf[128 * 128];
    if ((fd = open("/.history", O_RDONLY)) < 0) user_panic("Open .history failed");
    if ((r=read(fd, buf, (long)sizeof buf))<0) {
        user_panic("Read .history failed");
    }   
    close(fd);

    int i = 0, cmdi = 0;
    while(buf[i]) {
        int cmdj = 0;
        while(buf[i] && buf[i] != '\n') cmd[cmdi][cmdj++] = buf[i++];
        if (!buf[i]) break;
        ++i; ++cmdi;
    }   
    return cmdi;
}
