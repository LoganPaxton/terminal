# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -I./src

# Source files
SRC = src/lsh.c src/commands/reload.c src/commands/get.c src/commands/sudo.c src/read_line.c src/xor_encrypt.c src/create_syslog.c
OBJ = $(SRC:.c=.o)  # Converts .c files to .o object files

# Output executable
OUT = terminal

# Default target
all: $(OUT)

# Compile each source file into an object file
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Link the object files into the final executable
$(OUT): $(OBJ)
	$(CC) $(CFLAGS) -o $(OUT) $(OBJ)

# Clean the build files
clean:
	rm -f $(OUT) $(OBJ)

# Run the program
run: $(OUT)
	./$(OUT)

.PHONY: all clean run
