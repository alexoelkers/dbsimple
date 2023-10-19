#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct {
    char *buffer;          // pointer to buffer
    size_t buffer_length;  // size of buffer
    ssize_t input_length;  // bytes currently in buffer
} InputBuffer;

// construct a new input buffer
InputBuffer *new_input_buffer() {
    InputBuffer *input_buffer = malloc(sizeof(InputBuffer));
    input_buffer->buffer = NULL;
    input_buffer->buffer_length = 0;
    input_buffer->input_length = 0;

    return input_buffer;
}

// destroy an input buffer
void close_input_buffer(InputBuffer *input_buffer) {
    free(input_buffer->buffer);
    free(input_buffer);
}

// read stdin to InputBuffer
void read_input(InputBuffer *input_buffer) {
    ssize_t bytes_read = getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);

    if (bytes_read <= 0) {
        printf("Error reading input\n");
        exit(EXIT_FAILURE);
    }
    input_buffer->buffer[bytes_read - 1] = 0;     // remove new line character from buffer
    input_buffer->input_length = bytes_read - 1;  // account for removed newline character
}

// print REPL prompt
void print_prompt() { printf("db > "); }

int main(int argc, char **argv) {
    InputBuffer *input_buffer = new_input_buffer();
    while (true) {
        print_prompt();
        read_input(input_buffer);

        if (strcmp(input_buffer->buffer, ".exit") == 0) {
            close_input_buffer(input_buffer);
            exit(EXIT_SUCCESS);
        } else {
            printf("Unrecognised command: '%s'\n", input_buffer->buffer);
        }
    }
}