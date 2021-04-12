#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


void sort(int* array, int n) {
    int aux;
    for (int i = 0; i < n; i++) {
        for (int j = i; j < n; j++) {
            if (array[j] < array[i]) {
                aux = array[i];
                array[i] = array[j];
                array[j] = aux;
                sleep(1);
            }
        }
    }
}


int main(int argc, char **argv) {
    /*
    Input; n a_1 a_2 .... a_n
    */
    int n = atoi(argv[1]);
    int* array = calloc(n, sizeof(int));
    for (int i = 0; i < n; i++) {
        array[i] = atoi(argv[i + 2]);
    }
    sort(array, n);
    for (int i = 0; i < n; i++) {
        printf("%d\n", array[i]);
    }
    return 0;
}
