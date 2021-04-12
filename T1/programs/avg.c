#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>



int main(int argc, char **argv) {
    /*
    Input; n a_1 a_2 .... a_n
    */
    int n = (int) argv[0];
    float total = 0;
    int* array = calloc(n, sizeof(n));
    for (int i = 0; i < n; i++) {
        total += (int) argv[i + 1];
        sleep(1);
    }
    printf("%f", total / n);
    return 0;
}
