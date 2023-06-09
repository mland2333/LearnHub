#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void primes(int *p) {
    int a;
    pid_t pid;

    if ((pid = fork()) == 0) {
        read(p[0], &a, sizeof(int));
        sleep(1);
        exit(0);
    } else {
        while (read(p[0], &a, sizeof(int))) {
            printf("a = %d\n", a);
        }
        exit(0);
    }
}

int main() {
    int p[2];
    pipe(p);
    pid_t pid;

    if ((pid = fork()) == 0) {
        primes(p);
        exit(0);
    } else {
        for (int i = 2; i < 35; i++) {
            write(p[1], &i, sizeof(int));
        }
        close(p[1]);
        char buf[1];
        if (read(p[0], buf, 1) == 0) {
            exit(0);
        }
    }

    sleep(1);
    return 0;
}

