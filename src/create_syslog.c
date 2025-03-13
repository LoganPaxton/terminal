#include <stdio.h>

int create_syslog(char* command) {
    FILE *file;
    file = fopen(".system/.logs", "a");

    if (file == NULL) {
        perror("lsh");
        return 1;
    }

    fprintf(file, "%s", command);

    fclose(file);

    return 1;
}