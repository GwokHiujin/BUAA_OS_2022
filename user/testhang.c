#include "lib.h"

int isprime(int n) {
    int i;
    for (i = 2; i <= n - 1; i++) {
        if (n % i == 0)
            return 0;
    }
    return 1;
}

void getAllPrime(int n) {
    int i = 2;
    int checkpoint = n / 10;
    for (; i <= n; i++) {
        if (i % checkpoint == 0) {
            writef("\033[0m\033[1;32m@@@ Hey it is hanging @@@\n\033[0m");
        }
        if (isprime(i)) ;
    }
}


void umain() {
    writef("\033[0m\033[1;31m--- Start to test a slow nohup program ---\n\033[0m");
    getAllPrime(100000);
    writef("\033[0m\033[1;31m--- The nohup progame has done! ---\n\033[0m");
    return;
}
