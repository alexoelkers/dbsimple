#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "table.h"

typedef enum {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNISED_COMMAND
} MetaCommandResult;

typedef enum {
    PREPARE_SUCCESS,
    PREPARE_SYNTAX_ERROR,
    PREPARE_UNRECOGNISED_STATEMENT
} PrepareResult;

typedef enum {
    STATEMENT_INSERT,
    STATEMENT_SELECT
} StatementType;

typedef enum {
    EXECUTE_SUCCESS,
    EXECUTE_TABLE_FULL
} ExecuteResult;

typedef struct {
    char *buffer;          // pointer to buffer
    size_t buffer_length;  // size of buffer
    ssize_t input_length;  // bytes currently in buffer
} InputBuffer;

typedef struct {
    StatementType type;
    Row row_to_insert;  // only for insert
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
        int args_assigned = sscanf(input_buffer->buffer, "input %u %s %s",
                                   &(statement->row_to_insert.id),
                                   statement->row_to_insert.username,
                                   statement->row_to_insert.email);
        if (args_assigned > 3) {
            return PREPARE_SYNTAX_ERROR;
        }

        return PREPARE_SUCCESS;
    } else if (strcmp(input_buffer->buffer, "select") == 0) {
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }

    return PREPARE_UNRECOGNISED_STATEMENT;
}

ExecuteResult execute_insert(Statement *statement, Table *table) {
    if (table_full(table)) {
        return EXECUTE_TABLE_FULL;
    }

    Row *row_to_insert = &(statement->row_to_insert);

    serialise_row(row_to_insert, row_slot(table, table->num_rows));
    table->num_rows += 1;

    return EXECUTE_SUCCESS;
}

ExecuteResult execute_select(Statement *statement, Table *table) {
    Row row;
    for (uint32_t i = 0; i < table->num_rows; i++) {
        deserialise_row(row_slot(table, i), &row);
        print_row(&row);
    }
    return EXECUTE_SUCCESS;
}

ExecuteResult execute_statement(Statement *statement, Table *table) {
    switch (statement->type) {
        case (STATEMENT_INSERT):
            return execute_insert(statement, table);
        case (STATEMENT_SELECT):
            return execute_select(statement, table);
    }
}

// print REPL prompt
void print_prompt() { printf("db > "); }

int main(int argc, char **argv) {
    Table *table = new_table();
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
                break;
            case (PREPARE_SYNTAX_ERROR):
                printf("unable to parse statement: %s.\n",
                       input_buffer->buffer);
                continue;
            case (PREPARE_UNRECOGNISED_STATEMENT):
                printf("unrecognised keyword at start of: '%s'.\n",
                       input_buffer->buffer);
                continue;
        }

        switch (execute_statement(&statement, table)) {
            case (EXECUTE_SUCCESS):
                printf("Executed.\n");
                continue;
            case (EXECUTE_TABLE_FULL):
                printf("Error: Table full.\n");
                continue;
        }
    }
}