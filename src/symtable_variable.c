#include "symtable_variable.h"

SymbolVariable* symbol_table_variable_get_or_create(SymbolTable* table, const char* key)
{
    return (SymbolVariable*)symbol_table_get_or_create(table, key);
}

SymbolVariable* symbol_table_variable_get(SymbolTable* table, const char* key)
{
    return (SymbolVariable*)symbol_table_get(table, key);
}

SymbolVariable* symbol_table_function_get_or_create(SymbolTable* table, const char* key)
{
    return (SymbolFunction*)symbol_table_get_or_create(table, key);
}

SymbolVariable* symbol_table_function_get(SymbolTable* table, const char* key)
{
    return (SymbolFunction*)symbol_table_get(table, key);
}
