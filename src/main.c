#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef enum {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNISED_COMMAND
} MetaCommandResult;

typedef enum {
    PREPARE_SUCCESS,
    PREPARE_UNRECOGNISED_STATEMENT
} PrepareResult;

typedef enum {
    STATEMENT_INSERT,
    STATEMENT_SELECT
} StatementType;

typedef struct {
    char *buffer;          // pointer to buffer
    size_t buffer_length;  // size of buffer
    ssize_t input_length;  // bytes currently in buffer
} InputBuffer;

typedef struct {
    StatementType type;
} Statement;

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

MetaCommandResult do_meta_command(InputBuffer *input_butter) {
    if (strcmp(input_butter->buffer, ".exit") == 0) {
        close_input_buffer(input_butter);
        exit(EXIT_SUCCESS);
    } else {
        return META_COMMAND_UNRECOGNISED_COMMAND;
    }
}

PrepareResult prepare_statement(InputBuffer *input_buffer, Statement *statement) {
    if (strncmp(input_buffer->buffer, "insert", 6) == 0) {
        statement->type = STATEMENT_INSERT;
        return PREPARE_SUCCESS;
    } else if (strcmp(input_buffer->buffer, "select") == 0) {
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }

    return PREPARE_UNRECOGNISED_STATEMENT;
}

// print REPL prompt
void print_prompt() { printf("db > "); }

int main(int argc, char **argv) {
    InputBuffer *input_buffer = new_input_buffer();
    while (true) {
        print_prompt();
        read_input(input_buffer);

        if (input_buffer->buffer[0] == '.') {
            switch (do_meta_command(input_buffer)) {
                case (META_COMMAND_SUCCESS):
                    continue;
                case (META_COMMAND_UNRECOGNISED_COMMAND):
                    printf("Unrecognised command: '%s'.\n",
                           input_buffer->buffer);
                    continue;
            }
        }
        Statement statement;
        switch (prepare_statement(input_buffer, &statement)) {
            case (PREPARE_SUCCESS):
                continue;
            case (PREPARE_UNRECOGNISED_STATEMENT):
                printf("unrecognised keyword at start of: '%s'.\n",
                       input_buffer->buffer);
                continue;
        }
    }
}