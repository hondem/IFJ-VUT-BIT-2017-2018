#include "symtable_variable.h"

SymbolVariable* symbol_table_variable_get_or_create(SymbolTable* table, const char* key) {
    // internal!!!
    SymbolVariable* variable = (SymbolVariable*) symbol_table_get_or_create(table, key);

    return variable;
}

SymbolVariable* symbol_table_variable_get(SymbolTable* table, const char* key) {
    return (SymbolVariable*) symbol_table_get(table, key);
}

void symbol_variable_free_data(SymbolTableBaseItem* item) {
    NULL_POINTER_CHECK(item,);
    SymbolVariable* variable = (SymbolVariable*) item;
    memory_free(variable->base.key);
    variable->base.key = NULL;
}

void symbol_variable_init_data(SymbolTableBaseItem* item) {
    NULL_POINTER_CHECK(item,);
    SymbolVariable* variable = (SymbolVariable*) item;
    variable->frame = VARIABLE_FRAME_NONE;
    variable->scope_depth = 0;
}

SymbolVariable* symbol_variable_copy(SymbolVariable* variable) {
    NULL_POINTER_CHECK(variable, NULL);
    // deep copy of variable, without link to next
    SymbolVariable* new = memory_alloc(sizeof(SymbolVariable));
    new->data_type = variable->data_type;
    new->frame = variable->frame;
    new->scope_depth = variable->scope_depth;
    new->base.next = NULL;
    new->base.key = c_string_copy(variable->base.key);

    return new;
}
