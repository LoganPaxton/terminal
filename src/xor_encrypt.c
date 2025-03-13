#include <string.h>

#define KEY 'X'

void xor_encrypt(char *data, char key) {
    // Ensure `data` is a valid string (char pointer)
    if (data == NULL) {
        return; // Handle error if `data` is NULL
    }

    // Use size_t for the loop index, since strlen returns size_t
    for (size_t i = 0; i < strlen(data); i++) {
        data[i] ^= key;
    }
}
