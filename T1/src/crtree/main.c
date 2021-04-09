#include "process.h"


// CR-Tree simulator
int main(int argc, char **argv) {
    // TODO: Check args
    InputFile* file = read_file(argv[1]);
    create_process(file, atoi(argv[2]));
    wait(NULL);
    input_file_destroy(file);
    return 0;
}
