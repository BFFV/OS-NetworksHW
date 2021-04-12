#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main(int argc, char **argv) {
    /*
    Input; n a_1 a_2 .... a_n
    */
    int n = atoi(argv[1]);
    float total = 0;
    int* array = calloc(n, sizeof(int));
    for (int i = 1; i < n + 1; i++) {
        total += atoi(argv[i + 1]);
        sleep(1);
    }
    printf("%f\n", total / n);
    return 0;
}
