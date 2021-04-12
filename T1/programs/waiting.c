#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>



int main(int argc, char **argv) {
    /*
    Input; n a_1 a_2 .... a_n
    */
    printf("Ingresa un variable: ");
    char a;
    scanf("%c", &a);
    printf("\nIngresaste %c", a);
    return 0;
}
