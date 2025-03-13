#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include </workspaces/terminal/src/read_line.h>
#include </workspaces/terminal/src/xor_encrypt.h>
#include <sys/stat.h>
#include <dirent.h>

#define RL_BUFFSIZE 256
#define KEY 'X'

int lsh_sudo(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "lsh: an argument is required for sudo\n");
        return 1;
    }

    if (strcmp(args[1], "passwd") == 0) {  
        FILE *file = fopen(".system/.spwrd", "r");
        bool current_pword = false;

        if (file == NULL) {
            perror("lsh");
            return 1;
        }

        char line[256] = {0};  
        fgets(line, sizeof(line), file);
        fclose(file);

        line[strcspn(line, "\n")] = 0;

        if (line[0] == '\0') {  
            current_pword = true;
        }

        if (!current_pword) {
            printf("lsh: please enter your current password.\n> ");
            char *getPword = lsh_read_line();

            if (strcmp(getPword, "") == 0) {
                fprintf(stderr, "lsh: password cannot be blank\n");
                free(getPword);
                return 1;
            }

            char encrypted_input[256];
            strncpy(encrypted_input, getPword, sizeof(encrypted_input));
            xor_encrypt(encrypted_input, KEY);

            if (strcmp(encrypted_input, line) != 0) {
                fprintf(stderr, "lsh: incorrect password\n");
                free(getPword);
                return 1;
            }

            free(getPword);
        }

        printf("lsh: resetting password...\n");
        printf("lsh: please enter a new password\n> ");
        char *newPword = lsh_read_line();

        if (strcmp(newPword, "") == 0) {
            fprintf(stderr, "lsh: you must provide a non-blank password\n");
            free(newPword);
            return 1;
        }

        // Encrypt and reset file
        xor_encrypt(newPword, KEY);
        file = fopen(".system/.spwrd", "w");  // This will reset the file
        if (file == NULL) {
            perror("lsh");
            free(newPword);
            return 1;
        }
        fprintf(file, "%s", newPword);
        fclose(file);

        printf("lsh: password updated successfully\n");

        free(newPword);

        return 1;
    }

    if (strcmp(args[1], "reload")) {
        (void)args; // suppress unused parameter warning

        FILE *file = fopen(".system/.spwrd", "r");
            bool current_pword = false;
            (void)current_pword;

            if (file == NULL) {
                perror("lsh");
                return 1;
            }

        char line[256] = {0};  
            fgets(line, sizeof(line), file);
            fclose(file);

            line[strcspn(line, "\n")] = 0;

        printf("lsh: please enter your current password.\n> ");
        char *getPword = lsh_read_line();

        if (strcmp(getPword, "") == 0) {
            fprintf(stderr, "lsh: password cannot be blank\n");
            free(getPword);
            return 1;
        }

        char encrypted_input[256];
        strncpy(encrypted_input, getPword, sizeof(encrypted_input));
        xor_encrypt(encrypted_input, KEY);

        if (strcmp(encrypted_input, line) != 0) {
            fprintf(stderr, "lsh: incorrect password\n");
            free(getPword);
            return 1;
        }

        free(getPword);

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
            perror("lsh: failed to open .packages directory");
            return 1;
        }

        while ((entry = readdir(dir)) != NULL) {
            struct stat entry_stat;

            // Construct full file path
            char file_path[512];
            snprintf(file_path, sizeof(file_path), ".packages/%s", entry->d_name);

            // Get file stats
            if (stat(file_path, &entry_stat) == 0) {
                // Check if it is a regular file
                if (S_ISREG(entry_stat.st_mode)) {
                    char *filename = entry->d_name;
                    char *ext = strrchr(filename, '.');

                    if (ext && strcmp(ext, ".c") == 0) {  // Only compile .c files
                        char source_path[512], output_path[512], command[2048]; // Increased buffer size for command

                        snprintf(source_path, sizeof(source_path), ".packages/%s", filename);
                        snprintf(output_path, sizeof(output_path), ".packages/bin/%.*s", (int)(ext - filename), filename);

                        // Compile command
                        snprintf(command, sizeof(command), "gcc \"%s\" -o \"%s\" -w", source_path, output_path);

                        if (system(command) != 0) {
                            fprintf(stderr, "lsh: failed to compile %s\n", filename);
                        } else {
                            printf("Compiled: %s -> %s\n", source_path, output_path);
                        }
                    }
                }
            }
        }

        closedir(dir);
        return 1;
    }

    printf("lsh: sudo does not support the command '%s'\n", args[1]);

    return 1;
}
