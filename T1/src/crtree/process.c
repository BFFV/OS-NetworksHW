#include "process.h"


// Create a new process
int create_process(InputFile* file, int index) {
    int pid = fork();
    if (!pid) {
        char** current_process = file->lines[index];
        char* type_id = current_process[0];
        if (type_id[0] == 'W') {
            handle_worker(file, index);
        } else {
            handle_manager(file, index);
        }
    }
    return pid;
}

// Handle worker process workflow
void handle_worker(InputFile* file, int index) {
    char** current_process = file->lines[index];
    char* exec_name = current_process[1];
    int n_args = atoi(current_process[2]);
    char* args[n_args + 2]; // Program name + args
    args[0] = exec_name;
    for (int i = 0; i < n_args; i++) {
        args[i + 1] = current_process[3 + i];
    }
    args[n_args + 1] = NULL; // For execvp convention

    // Use a child process for program execution
    int pid = fork();
    clock_t start = clock();
    if (!pid) {
        execvp(exec_name, args); // Program execution
    } else {
        //kill(pid, SIGTERM); // TODO: Interruptions
        //kill(pid, SIGKILL);
        int status;
        wait(&status);
        int exit_code = WEXITSTATUS(status); // Exit code
        clock_t end = clock();
        double time_spent = (double) (end - start) / CLOCKS_PER_SEC; // Execution time

        // TODO: Write Output
    }
    exit(0);
}

// Handle manager process workflow
void handle_manager(InputFile* file, int index) {
    char** current_process = file->lines[index];
    int timeout = atoi(current_process[1]);
    int n_children = atoi(current_process[2]);
    int n_active = n_children;
    int* indexes = malloc(sizeof(int) * n_children); // File index for every child process
    for (int c = 0; c < n_children; c++) {
        indexes[c] = atoi(current_process[3 + c]);
    }
    int* children = malloc(sizeof(int) * n_children); // Pid for every child process
    for (int index = 0; index < n_children; index++) {
        children[index] = create_process(file, indexes[index]);
    }

    // Timer
    int timer_pid = fork();
    if (!timer_pid) {
        sleep(timeout);
        exit(0);
    }

    // Wait for children/timer
    // TODO: Intercept signals (R/M)
    bool monitoring = true;
    while (monitoring) {
        pid_t exited = wait(NULL);
        if (exited == timer_pid) {
            // TODO: Send signal for timeout
            printf("Timeout Reached!\n");
        } else {
            n_active--;
        }
        if (!n_active) {
            monitoring = false;
        }
    }

    // TODO: Write output

    // Free heap memory
    free(indexes);
    free(children);
    exit(0);
}

// TODO: Write Output functions
