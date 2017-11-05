#ifndef _SYMTABLE_FUNCTION_H
#define _SYMTABLE_FUNCTION_H

#include "common.h"
#include "symtable.h"
#include "data_type.h"
#include "dynamic_string.h"

#define symbol_table_function_init(bucket_count) symbol_table_init( \
    bucket_count, \
    sizeof(SymbolFunction), \
    symbol_function_init_data, \
    symbol_function_free_data \
)

typedef struct symbol_function_param_t {
    // inherit from symbolvariable
    char* name;
    DataType data_type;
    struct symbol_function_param_t* next;
    struct symbol_function_param_t* prev;
} SymbolFunctionParam;

typedef struct symbol_function_t {
    SymbolTableBaseItem base;

    bool declared;
    bool defined;
    DataType return_data_type;
    size_t arguments_count;
    SymbolFunctionParam* param;
    SymbolFunctionParam* param_tail;
} SymbolFunction;

SymbolFunction* symbol_table_function_get_or_create(SymbolTable* table, const char* key);

SymbolFunction* symbol_table_function_get(SymbolTable* table, const char* key);

void symbol_function_init_data(SymbolTableBaseItem* item);

void symbol_function_free_data(SymbolTableBaseItem* item);

SymbolFunctionParam* symbol_function_add_param(SymbolFunction* function, char* name, DataType data_type);

SymbolFunctionParam* symbol_function_get_param(SymbolFunction* function, size_t index);

SymbolFunction* symbol_function_find_declared_function_without_definition(SymbolTable* table);

String* symbol_function_generate_function_label(SymbolFunction* function);

String* symbol_function_get_param_name_alias(SymbolFunction* function, SymbolFunctionParam* param);

#endif // _SYMTABLE_FUNCTION_H
