#include "process.h"

// For signal handling
volatile sig_atomic_t tree = 0;

// Pass console interruptions to CR-Tree if needed
void handle_signals(int signum) {
    if (tree) {
        kill(tree, signum);
    }
}

// CR-Tree simulator
int main(int argc, char **argv) {
    // TODO: Check args
    signal(SIGINT, handle_signals);
    signal(SIGABRT, handle_signals);
    InputFile* file = read_file(argv[1]);
    tree = create_process(file, atoi(argv[2]), NULL, NULL);
    wait(NULL);
    tree = 0;
    input_file_destroy(file);
    return 0;
}
