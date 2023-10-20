#ifndef TABLE_H_
#define TABLE_H_

#include <stdint.h>

#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 256

typedef struct {
    uint32_t id;
    char username[COLUMN_USERNAME_SIZE];
    char email[COLUMN_EMAIL_SIZE];
} Row;

// table constants
#define TABLE_MAX_PAGES 100

typedef struct {
    uint32_t num_rows;
    void *pages[TABLE_MAX_PAGES];
} Table;

void *row_slot(Table *table, uint32_t row_num);
void serialise_row(Row *source, void *destination);
void deserialise_row(void *source, Row *destination);
void print_row(Row *row);

Table *new_table();
void free_table(Table *table);
int table_full(Table *table);

#endif