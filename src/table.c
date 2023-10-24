#include "table.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// row definitions
#define size_of_attribute(Struct, Attribute) sizeof(((Struct *)0)->Attribute)
const uint32_t ID_SIZE = size_of_attribute(Row, id);
const uint32_t USERNAME_SIZE = size_of_attribute(Row, username);
const uint32_t EMAIL_SIZE = size_of_attribute(Row, email);
const uint32_t ID_OFFSET = 0;
const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;
const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;

const uint32_t PAGE_SIZE = 4096;
const uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
const uint32_t TABLE_MAX_ROWS = TABLE_MAX_PAGES * ROWS_PER_PAGE;

Table *new_table() {
    Table *table = malloc(sizeof(Table));
    table->num_rows = 0;

    for (uint8_t i = 0; i < TABLE_MAX_PAGES; i++) {
        table->pages[i] = NULL;
    }
    return table;
}

void free_table(Table *table) {
    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
        free(table->pages[i]);
    }
    free(table);
}

int table_full(Table *table) {
    if (table->num_rows >= TABLE_MAX_ROWS) {
        return 1;
    }
    return 0;
}

void *row_slot(Table *table, uint32_t row_num) {
    uint32_t page_num = row_num / ROWS_PER_PAGE;
    void *page = table->pages[page_num];
    if (page == NULL) {
        // allocate a new row iff page is empty
        page = table->pages[page_num] = malloc(PAGE_SIZE);
    }
    uint32_t row_offset = row_num % ROWS_PER_PAGE;
    uint32_t byte_offset = row_offset * ROW_SIZE;
    printf("row pointer: %p\n", (page + byte_offset));
    return page + byte_offset;
}

void serialise_row(Row *source, void *destination) {
    printf("row pointer: %p\n", destination);
    memcpy((destination + ID_OFFSET), &(source->id), ID_SIZE);  // ID
    memcpy((destination + USERNAME_OFFSET), &(source->username), USERNAME_SIZE);
    memcpy((destination + EMAIL_OFFSET), &(source->email), EMAIL_SIZE);
}

void deserialise_row(void *source, Row *destination) {
    printf("row pointer: %p\n", source);
    memcpy(&(destination->id), (source + ID_OFFSET), ID_SIZE);  // ID
    memcpy(&(destination->username), (source + USERNAME_OFFSET), USERNAME_SIZE);
    memcpy(&(destination->email), (source + EMAIL_OFFSET), EMAIL_SIZE);
}

void print_row(Row *row) {
    printf("(%u, %s, %s)\n", row->id, row->username, row->email);
}