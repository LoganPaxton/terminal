#ifndef SUDO_H
#define SUDO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include </workspaces/terminal/src/read_line.h>


#define RL_BUFFSIZE 256
#define KEY 'X'

int lsh_sudo(char **args);

#endif