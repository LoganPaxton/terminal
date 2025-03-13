#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int lsh_get(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "lsh: expected arugment");
        return 1;
    }

    char* url = args[1];

    const char *filename = strrchr(url, '/');
    if (filename) {
        filename++;
    } else {
        filename = url;
    }
    
    char command[512];
    snprintf(command, sizeof(command), "wget --show-progress %s -O %s", url, filename);
    system(command);

    char newPath[512];
    snprintf(newPath, sizeof(newPath), ".packages/%s", filename);
    if (rename(filename, newPath) != 0) {
        perror("lsh");
        return 1;
    }

    return 1;
}