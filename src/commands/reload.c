#include <stdio.h>

#define KEY 'X'

int lsh_reload(char **args) {
    (void)args; // suppress unused parameter warning

    printf("lsh: This version of the command 'reload' is deprecated. Please use 'sudo reload'. This command will be removed in the next version update.\n");

    return 1;
}
