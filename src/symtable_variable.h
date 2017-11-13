#ifndef SYMTABLE_VARIABLE_H
#define SYMTABLE_VARIABLE_H

#include "symtable.h"
#include "symtable_function.h"
#include "data_type.h"
#include "shared_pointer.h"

typedef enum {
    VARIABLE_FRAME_NONE,
    VARIABLE_FRAME_LOCAL,
    VARIABLE_FRAME_GLOBAL,
    VARIABLE_FRAME_TEMP,
} SymbolVariableFrame;

typedef enum {
    VARIABLE_META_TYPE_PURE = 0,
    VARIABLE_META_TYPE_DYNAMIC = 1,
    VARIABLE_META_TYPE_PRINTED = 2,
    VARIABLE_META_TYPE_NON_OPTIMIZABLE = 3
} SymbolVariableMetaType;

typedef struct {
    size_t occurrences_count;
    int type;
} SymbolVariableMetaData;

typedef struct symbol_variable_t {
    SymbolTableBaseItem base;
    char* alias_name;

    DataType data_type;
    SymbolVariableFrame frame;

    size_t scope_depth;
    char* scope_alias;

    SymbolVariableMetaData* meta_data;
} SymbolVariable;

/**
 * @internal
 */
SymbolVariable* symbol_table_variable_get_or_create(SymbolTable* table, const char* key);

/**
 * Try to find variable in given symbol table by name.
 * @param table symbol table
 * @param key key to find
 * @return instance or NULL
 */
SymbolVariable* symbol_table_variable_get(SymbolTable* table, const char* key);

/**
 * Initialize data for single item of table.
 * @param item item to initialize
 */
void symbol_variable_init_data(SymbolTableBaseItem* item);

/**
 * Free additional symbol variable data, but not variable itself.
 * @param item
 */
void symbol_variable_free_data(SymbolTableBaseItem* item);

/**
 * Copy all symbol variable data to new created.
 * @param variable
 * @return
 */
SymbolVariable* symbol_variable_copy(SymbolVariable* variable);

/**
 * Initialize STANDALONE symbol variable from function parameter - with manually indexed name.
 * @param function function to parameter
 * @param param source param
 * @return created symbol variable
 */
SymbolVariable* symbol_variable_init_from_function_param(SymbolFunction* function, SymbolFunctionParam* param);

/**
 * Free STANDALONE symbol variable (not from existing symbol table).
 * @param variable
 */
void symbol_variable_single_free(SymbolVariable** variable);

#define symbol_table_variable_init(bucket_count) symbol_table_init( \
    bucket_count, \
    sizeof(SymbolVariable), \
    symbol_variable_init_data, \
    symbol_variable_free_data)

#define symbol_variable_init(key) (SymbolVariable *) symbol_table_new_item( \
    key, \
    sizeof(SymbolVariable) \
)

SymbolVariableMetaData* symbol_variable_meta_data_init();
void symbol_variable_meta_data_free(SymbolVariableMetaData** item);
void symbol_variable_meta_data_item_free(LListBaseItem* item);

void symbol_variable_meta_data_add_reference(SymbolVariableMetaData* item);
void symbol_variable_meta_data_remove_reference(SymbolVariableMetaData* item);

#endif // SYMTABLE_VARIABLE_H

