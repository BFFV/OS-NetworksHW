#include "mlfq.h"


// Scheduler Simulator
int main(int argc, char **argv) {

    // Check input
    if (argc < 6) {
        printf("[Error] Not enough arguments were provided!\n");
        return 1;
    }

    // Receive args
    InputFile* file = read_file(argv[1]);
    int k = file->len;
    int n_queues = atoi(argv[3]);
    int q = atoi(argv[4]);
    int S = atoi(argv[5]);

    // Initialize program
    Process** process_array = load_processes(file);
    input_file_destroy(file);
    Queue** queue_array = create_queues(n_queues);
    int t = 0;
    int index = 0;
    bool running = true;
    int finished_counter = 0;
    Process* cpu_exec = NULL;

    // Simulation
    while (running) {

        // Reset all queues
        if (!S) {
            S = atoi(argv[5]);
            for (int i = 1; i < n_queues; i++) {
                if (queue_array[i]->length) {
                    merge(queue_array[i], queue_array[0]);
                }
            }
        }

        // Add waiting time & check processes in waiting
        for (int p = 0; p < k; p++) {
            Process* current_process = process_array[p];
            if (current_process->start_time < t && !current_process->finished && !current_process->executing) { // Add waiting time
                current_process->waiting_time++;
            }
            if (current_process->waiting) { // Check processes in waiting
                current_process->wait--;
                if (!current_process->wait) {
                    current_process->waiting = false;
                    current_process->wait = current_process->initial_wait;
                    push(queue_array[current_process->current_queue], current_process);
                }
            }
        }

        // Start new processes
        bool loading = true;
        while (loading && index < k) {
            if (process_array[index]->start_time == t) {
                push(queue_array[0], process_array[index]);
                index++;
            } else {
                loading = false;
            }
        }

        // CPU is busy
        if (cpu_exec != NULL) {
            cpu_exec->cycles--;
            cpu_exec->quantum--;
            if (!cpu_exec->cycles) { // Process is finished
                finished_counter++;
                cpu_exec->executing = false;
                cpu_exec->finished = true;
                cpu_exec->turnaround = t - cpu_exec->start_time;
                cpu_exec = NULL;
            } else if (!cpu_exec->quantum) { // Process interruption
                if (cpu_exec->current_queue < n_queues - 1) {
                    cpu_exec->current_queue++;
                }
                cpu_exec->int_count++;
                cpu_exec->executing = false;
                push(queue_array[cpu_exec->current_queue], cpu_exec);
                cpu_exec = NULL;
            } else if (cpu_exec->initial_wait && cpu_exec->wait) { // Process I/O
                cpu_exec->wait--;
                if (!cpu_exec->wait) { // Move to waiting
                    if (cpu_exec->current_queue) {
                        cpu_exec->current_queue--;
                    }
                    cpu_exec->executing = false;
                    cpu_exec->wait = cpu_exec->waiting_delay;
                    cpu_exec->waiting = true;
                    cpu_exec = NULL;
                }
            }
        }

        // CPU can attend new process
        if (cpu_exec == NULL) {
            Queue* queue = NULL;
            for (int i = 0; i < n_queues; i++) { // Get top queue that contains processes
                if (queue == NULL && queue_array[i]->length) {
                    queue = queue_array[i];
                }
            }
            if (queue != NULL) { // READY is not empty
                Process* selected_process = pop(queue);
                selected_process->quantum = (n_queues - queue->priority) * q;
                selected_process->exec_count++;
                selected_process->executing = true;
                if (selected_process->response == -1) {
                    selected_process->response = t - cpu_exec->start_time;
                }
                cpu_exec = selected_process; // Process enters the CPU
            } else if (finished_counter == k) { // All processes are finished
                running = false;
            }
        }

        // Next tick
        S--;
        t++;
    }

    // Write output & free memory
    destroy_queues(queue_array, n_queues);
    char* output_name = argv[2];
    write_output(process_array, k, output_name);
    destroy_processes(process_array, k);

    return 0;
}
