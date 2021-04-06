#include "process.h"


int main(int argc, char **argv) {
  // Check args
  InputFile* file = read_file(argv[1]);
  int pid = create_process(file, atoi(argv[2]));
  if (pid) {
    printf("Returned %d\n", pid);
    //wait(NULL);
    input_file_destroy(file);
  }
  return 0;
}
