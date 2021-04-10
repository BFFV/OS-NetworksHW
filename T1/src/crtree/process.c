#include "process.h"

// For signal handling
volatile sig_atomic_t worker_child = 0;
volatile sig_atomic_t interrupted = 0;

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
        exit(0);
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
        char* arg = current_process[3 + i];
        arg = strtok(arg, " \t\r\n\f\v"); // Strip
        args[i + 1] = arg;
    }
    args[n_args + 1] = NULL; // For execvp convention

    // Use a child process for program execution
    int pid = fork();
    time_t start = time(NULL);
    if (!pid) {
        execvp(exec_name, args); // Program execution
    }

    // Catch signals
    worker_child = pid; // Save child pid for interruptions
    signal(SIGINT, SIG_IGN);
    signal(SIGABRT, abort_worker);

    // Statistics
    int status;
    wait(&status);
    worker_child = 0; // Remove child pid for interruptions
    char* interruption = "0"; // Check if process was interrupted
    if (interrupted) {
        interruption = "1";
    }
    int exit_code = WEXITSTATUS(status); // Exit code
    time_t end = time(NULL);
    int time_spent = end - start; // Execution time

    // Write worker output
    char* output[n_args + 5];
    for (int i = 0; i < n_args + 1; i++) {
        output[i] = args[i];
    }
    int time_length = snprintf(NULL, 0, "%d", time_spent); // Double to Str
    char time_spent_string[time_length];
    sprintf(time_spent_string, "%d", time_spent);
    output[n_args + 1] = time_spent_string;
    int exit_length = snprintf(NULL, 0, "%d", exit_code); // Int to Str
    char exit_string[exit_length];
    sprintf(exit_string, "%d", exit_code);
    output[n_args + 2] = exit_string;
    output[n_args + 3] = interruption;
    output[n_args + 4] = NULL;
    char** file_content[1] = { output };
    int index_length = snprintf(NULL, 0, "%d", index); // Int to Str
    char p_index[index_length];
    sprintf(p_index, "%d", index);
    write_output_file(p_index, file_content, 1); // Write output file
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
}

// SIGABRT handler for worker process
void abort_worker(int signum) {
    if (worker_child) {
        kill(worker_child, SIGABRT);
        interrupted = 1;
    }
}
