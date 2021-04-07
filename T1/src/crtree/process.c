#include "process.h"


/*
Por hacer:
    - Como pasar las estadisticas desde un worker a un manager: cada worker escribe directo
    en un archivo? - create_process y los handler retornan un string
    - Timeout del manager -> Como manejar 

*/

int create_process(InputFile* file, int index) {
    int pid = fork();
    if (!pid) {
        char** current_process = file->lines[index];
        char* type_id = current_process[0];
        if (type_id[0] == 'W') {
            handle_worker(file, index);
        } else if (type_id[0] == 'M') {
            handle_manager(file, index);
        } else {
            // call root function
            printf("Manager Root\n");
        }
        // Evitamos volver al main, retornamos de inmediato
        exit(0);
    } else {
        //pid_t exited = wait(NULL);
        // printf("Child Exited: %d\n", exited);
    }
    return pid;
}

void handle_worker(InputFile* file, int index) {
    char** current_process = file->lines[index];
    char* exec_name = current_process[1];
    int n_args = atoi(current_process[2]);
    char* args[n_args + 2];
    args[0] = exec_name;
    for (int i = 0; i < n_args;i++) {
        args[i + 1] = current_process[3 + i];
    }
    args[n_args + 1] = NULL;
    // Esto lo hago para poder tener estadísticas
    int pid = fork(); // Sera el hijo quien haga execvp
    clock_t start = clock();
    if (!pid) {
        exit(execvp(exec_name, args));
    } else {
        int status;
        wait(&status);
        clock_t end = clock();
        double time_spent = (double) (end -start) / CLOCKS_PER_SEC;

    }
}

void handle_manager(InputFile* file, int index) {
    char** current_process = file->lines[index];
    int timeout = atoi(current_process[1]);
    int n_children = atoi(current_process[2]);
    int* children = malloc(sizeof(int) * n_children);
    int* children_pid = malloc(sizeof(int) * n_children);
    for (int i = 0; i < n_children; i++) {
        children[i] = atoi(current_process[3 + i]);
    }
    // Inicio el timer
    int time_pid = fork();
    if (!time_pid) {
        sleep(timeout);
        // Exit para no seguir bajando en el codigo
        exit(0);
    }
    for (int index=0; index < n_children; index++) {
        children_pid[index] = create_process(file, children[index]);
    }

    wait(NULL);
    free(children);
}
