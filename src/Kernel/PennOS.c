#include "kernel.h"

int main(int argc, char *argv[]) {
    // spawn a process for shell
    k_shell_create();

    return 0;
}
