#include "process.h"


int create_process(InputFile* file, int index) {
    int pid = fork();
    if (!pid) {
        char** current_process = file->lines[index];
        char* type_id = current_process[0];
        if (type_id[0] == 'W') {
            char* exec_name = current_process[1];
            handle_worker(exec_name, &current_process[3]);
        } else if (type_id[0] == 'M') {
            // call manager function
            printf("Manager\n");
            int timeout = atoi(current_process[1]);
            int n_children = atoi(current_process[2]);
            int* children = malloc(sizeof(int) * n_children);
            for (int i = 0; i < n_children; i++) {
                children[i] = atoi(current_process[3 + i]);
            }
            handle_manager(file, timeout, n_children, children);
        } else {
            // call root function
            printf("Manager Root\n");
        }
    } else {
        pid_t exited = wait(NULL);
        printf("Child Exited: %d\n", exited);
    }
    return pid;
}

void handle_worker(char* codename, char** args) {
    execvp(codename, args);
}

void handle_manager(InputFile* file, int timeout, int child_count, int* children) {
    for (int index=0; index < child_count; index++) {
        create_process(file, children[index]);
    }
    // wait(NULL);
    free(children);
}