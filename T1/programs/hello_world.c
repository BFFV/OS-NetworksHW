#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv) {
    for (int c=0; c < argc; c++) {
        printf("Hello World: %s\n", argv[c]);
    }
    sleep(40);
    return 0;
}
