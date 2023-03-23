#include "kernel.h"

int main(int argc, char *argv[]) {
    // spawn a process for shell
    pcb_t *shell_process = k_shell_create();

    setcontext(&(shell_process)->context);

    return 0;
}
