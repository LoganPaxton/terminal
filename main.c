#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dirent.h>

#define LSH_RL_BUFSIZE 1024
#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"

// Function declarations for built-in shell commands
int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);
int lsh_reload(char **args);

// List of built-in commands
char *builtin_str[] = {
    "cd",
    "help",
    "exit",
    "reload"
};

int (*builtin_func[]) (char **) = {
    &lsh_cd,
    &lsh_help,
    &lsh_exit,
    &lsh_reload
};

int lsh_num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
}

// Built-in function implementations
int lsh_cd(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "lsh: expected argument to \"cd\"\n");
    } else {
        if (chdir(args[1]) != 0) {
            perror("lsh");
        }
    }
    return 1;
}

int lsh_help(char **args) {
    printf("Logan's Simple Shell\n");
    printf("Type program names and arguments, then hit enter.\n");
    printf("The following are built-in commands:\n");

    for (int i = 0; i < lsh_num_builtins(); i++) {
        printf("  %s\n", builtin_str[i]);
    }

    printf("Use the 'reload' command to compile and run C programs from .packages/\n");
    return 1;
}

int lsh_exit(char **args) {
    return 0;
}

int lsh_reload(char **args) {
    printf("Reloading terminal...\n");

    // Clear screen
    printf("\033[H\033[J");

    // Ensure .packages/bin directory exists
    struct stat st;
    if (stat(".packages/bin", &st) != 0) {
        if (mkdir(".packages/bin", 0700) != 0) {
            perror("lsh");
            return 1;
        }
    }

    DIR *dir;
    struct dirent *entry;

    dir = opendir(".packages");
    if (dir == NULL) {
        perror("lsh: failed to open .packages");
        return 1;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {  // Only regular files
            char *filename = entry->d_name;
            char *ext = strrchr(filename, '.');

            if (ext && strcmp(ext, ".c") == 0) {  // Only compile .c files
                char source_path[512], output_path[512], command[2048]; // Increased buffer size for command

                snprintf(source_path, sizeof(source_path), ".packages/%s", filename);
                snprintf(output_path, sizeof(output_path), ".packages/bin/%.*s", (int)(ext - filename), filename);

                // Increased buffer size for the command
                snprintf(command, sizeof(command), "gcc \"%s\" -o \"%s\" -w", source_path, output_path);

                if (system(command) != 0) {
                    fprintf(stderr, "lsh: failed to compile %s\n", filename);
                } else {
                    printf("Compiled: %s -> %s\n", source_path, output_path);
                }
            }
        }
    }

    closedir(dir);
    return 1;
}

// Function to launch processes
int lsh_launch(char **args) {
    pid_t pid, wpid;
    int status;

    pid = fork();
    if (pid == 0) {
        if (execvp(args[0], args) == -1) {
            perror("lsh");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("lsh");
    } else {
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

// Function to execute built-in or external commands
int lsh_execute(char **args) {
    if (args[0] == NULL) {
        return 1;
    }

    for (int i = 0; i < lsh_num_builtins(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }

    // Check if command exists in .packages/bin/
    char program_path[256];
    snprintf(program_path, sizeof(program_path), ".packages/bin/%s", args[0]);

    if (access(program_path, X_OK) == 0) {  // If executable
        pid_t pid = fork();
        if (pid == 0) {
            execv(program_path, args);
            perror("lsh");
            exit(EXIT_FAILURE);
        } else if (pid < 0) {
            perror("lsh");
        } else {
            int status;
            waitpid(pid, &status, 0);
        }
        return 1;
    }

    return lsh_launch(args);  // Default shell execution
}

// Function to read input line
char *lsh_read_line(void) {
    char *line = NULL;
    size_t bufsize = 0;
    getline(&line, &bufsize, stdin);
    return line;
}

// Function to split input into tokens
char **lsh_split_line(char *line) {
    int bufsize = LSH_TOK_BUFSIZE;
    int position = 0;
    char **tokens = malloc(bufsize * sizeof(char *));
    char *token;

    if (!tokens) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, LSH_TOK_DELIM);
    while (token != NULL) {
        tokens[position] = token;
        position++;

        if (position >= bufsize) {
            bufsize += LSH_TOK_BUFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char *));
            if (!tokens) {
                fprintf(stderr, "lsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, LSH_TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}

// Main shell loop
void lsh_loop(void) {
    char *line;
    char **args;
    int status;

    do {
        printf("> ");
        line = lsh_read_line();
        args = lsh_split_line(line);
        status = lsh_execute(args);

        free(line);
        free(args);
    } while (status);
}

// Main function
int main(int argc, char **argv) {
    lsh_loop();
    return EXIT_SUCCESS;
}
