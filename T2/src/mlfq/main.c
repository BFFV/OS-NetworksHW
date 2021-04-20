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
    Process* cpu_exec = NULL;

    // Simulation
    while (running) {
        // TODO: Re-enter processes that are done waiting, refreshing their wait

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
                cpu_exec = NULL;
            } else if (!cpu_exec->quantum) { // Process interruption
                // TODO: Drop process 1 queue
                cpu_exec = NULL;
            } else if (cpu_exec->initial_wait && cpu_exec->wait) { // Process I/O
                cpu_exec->wait--;
                if (!cpu_exec->wait) {
                    // TODO: Move to Waiting
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
                cpu_exec = selected_process; // Process enters the CPU
            } else if (index == k) { // All processes are finished
                running = false;
            }
        }

        // Next tick
        t++;
    }

    // Write output & free memory
    destroy_queues(queue_array, n_queues);
    char* output_name = argv[2];
    // TODO: Write output csv
    destroy_processes(process_array, k);

    return 0;
}
