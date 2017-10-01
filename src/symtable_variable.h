#ifndef SYMTABLE_VARIABLE_H
#define SYMTABLE_VARIABLE_H

#include "symtable.h"

typedef struct symbol_variable_t {
    SymbolTableBaseItem base;
    // TODO: define all needed members
    bool declared;
    bool defined;
    short data_type;
} SymbolVariable;

SymbolVariable* symbol_table_variable_get_or_create(SymbolTable* table, const char* key);
SymbolVariable* symbol_table_variable_get(SymbolTable* table, const char* key);

void symbol_variable_free_data(SymbolTableBaseItem* item);

#define symbol_table_variable_init(bucket_count) symbol_table_init( \
    bucket_count, \
    sizeof(SymbolVariable), \
    NULL, \
    symbol_variable_free_data)

#endif // SYMTABLE_VARIABLE_H

