#include "process.h"

// For signal handling
volatile sig_atomic_t worker_child = 0;
volatile sig_atomic_t interrupted = 0;
volatile sig_atomic_t* manager_children = NULL;
volatile sig_atomic_t manager_child_count = 0;

// Create a new process
int create_process(InputFile* file, int index, int* indexes, int* children) {
    int pid = fork();
    if (!pid) {
        // Free inherited heap memory
        if (indexes != NULL) {
            free(indexes);
            free(children);
        }
        if (file->lines[index][0][0] == 'W') {
            handle_worker(file, index);
        } else {
            handle_manager(file, index);
        }
        input_file_destroy(file);
        exit(0);
    }
    return pid;
}

// Handle worker process workflow
void handle_worker(InputFile* file, int index) {
    char** current_process = file->lines[index];
    char* exec_name = current_process[1];
    int n_args = atoi(current_process[2]);
    char** args = malloc((n_args + 2) * sizeof(char*)); // Program name + args
    args[0] = calloc(strlen(exec_name) + 1, sizeof(char));
    strcpy(args[0], exec_name);
    for (int i = 0; i < n_args; i++) {
        char* arg = strtok(current_process[3 + i], " \t\r\n\f\v"); // Strip
        args[i + 1] = calloc(strlen(arg) + 1, sizeof(char));
        strcpy(args[i + 1], arg);
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
    char** output = malloc((n_args + 5) * sizeof(char*));
    for (int i = 0; i < n_args + 1; i++) {
        output[i] = calloc(strlen(args[i]) + 1, sizeof(char));
        strcpy(output[i], args[i]);
        free(args[i]); // Free args that will not be reused
    }
    free(args[n_args + 1]);
    free(args);
    int time_length = snprintf(NULL, 0, "%d", time_spent); // Int to Str
    char time_spent_string[time_length];
    sprintf(time_spent_string, "%d", time_spent);
    output[n_args + 1] = calloc(strlen(time_spent_string) + 1, sizeof(char));
    strcpy(output[n_args + 1], time_spent_string);
    int exit_length = snprintf(NULL, 0, "%d", exit_code); // Int to Str
    char exit_string[exit_length];
    sprintf(exit_string, "%d", exit_code);
    output[n_args + 2] = calloc(strlen(exit_string) + 1, sizeof(char));
    strcpy(output[n_args + 2], exit_string);
    output[n_args + 3] = calloc(strlen(interruption) + 1, sizeof(char));
    strcpy(output[n_args + 3], interruption);
    output[n_args + 4] = NULL;
    char** file_content[1] = { output };
    int index_length = snprintf(NULL, 0, "%d", index); // Int to Str
    char p_index[index_length];
    sprintf(p_index, "%d", index);
    write_output_file(p_index, file_content, 1); // Write output file

    // Free output heap memory
    for (int i = 0; i < n_args + 5; i++) {
        free(output[i]);
    }
    free(output);
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
        children[index] = create_process(file, indexes[index], indexes, children);
    }
    manager_child_count = n_children;
    manager_children = children; // Point to children in the global scope

    // Timer
    int timer_pid = fork();
    if (!timer_pid) {
        free(indexes);
        free(children);
        input_file_destroy(file);
        sleep(timeout);
        exit(0);
    }

    // Catch signals
    if (current_process[0][0] == 'R') {
        signal(SIGINT, abort_manager);
    } else {
        signal(SIGINT, SIG_IGN);
    }
    signal(SIGABRT, abort_manager);

    // Wait for children/timer
    bool monitoring = true;
    while (monitoring) {
        pid_t exited = wait(NULL);
        if (exited == timer_pid) { // Timeout reached
            raise(SIGABRT);
        } else {
            n_active--;
        }
        if (!n_active) { // All children have exited
            monitoring = false;
        }
    }
    kill(timer_pid, SIGABRT); // Stop timer process if needed
    manager_child_count = 0;
    manager_children = NULL; // Stop pointing to children

    // Write manager output
    int index_length = snprintf(NULL, 0, "%d", index); // Int to Str
    char output_index[index_length];
    sprintf(output_index, "%d", index);
    long length;
    FILE* output_file = fopen(strcat(output_index, ".txt"), "w");
    for (int i = 0; i < n_children; i++) {
        // Copy child output to buffer
        int child_index_length = snprintf(NULL, 0, "%d", indexes[i]); // Int to Str
        char child_index[child_index_length];
        sprintf(child_index, "%d", indexes[i]);
        FILE* child_file  = fopen(strcat(child_index, ".txt"), "r"); // Open child file
        fseek(child_file, 0, SEEK_END);
        length = ftell(child_file);
        fseek(child_file, 0, SEEK_SET);
        char* buffer = calloc(length + 1, sizeof(char));
        fread(buffer, 1, length, child_file);

        // Copy buffer to outfile
        fputs(buffer, output_file);
        fclose(child_file);
        free(buffer);
    }
    fclose(output_file);

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

// SIGABRT handler for manager process
void abort_manager(int signum) {
    if (manager_children != NULL) {
        for (int c = 0; c < manager_child_count; c++) {
            kill(manager_children[c], SIGABRT);
        }
    }
}
