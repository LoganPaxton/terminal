#include "read_line.h"
#include <stdlib.h>
#include <sys/types.h>

char *lsh_read_line(void) {
    char *line = NULL;
    size_t bufsize = 0;
    ssize_t len = getline(&line, &bufsize, stdin);
    
    if (len > 0 && line[len - 1] == '\n') {
        line[len - 1] = '\0';
    }

    return line;
}
