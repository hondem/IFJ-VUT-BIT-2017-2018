#ifndef _SYMBOL_REGISTER_H
#define _SYMBOL_REGISTER_H

#include "symtable.h"

/**
 * @brief Register of used functions/variables.
 */
typedef struct symbol_register_t {
    SymbolTableSymbolFunction* functions;
    SymbolTableSymbolVariable* variables;
} SymbolRegister;

/**
 * Initialize symbol register with both tables.
 * @return initialized register
 */
SymbolRegister* symbol_register_init();

/**
 * Free symbol register with both tables.
 * @param register_ Symbol register
 */
void symbol_register_free(SymbolRegister** register_);

/**
 * Add new variables table and actual push backward.
 * @param register_ Symbol register
 */
void symbol_register_push_variables_table(SymbolRegister* register_);

/**
 * Remove actual variables table and second table marks as actual.
 * @param register_ Symbol register
 */
void symbol_register_pop_variables_table(SymbolRegister* register_);

/**
 * Try to find variable by name in actual variables table.
 * @param register_ Symbol register
 * @param key variable name to find
 * @return Found variable symbol or NULL
 */
SymbolVariable* symbol_register_find_variable(SymbolRegister* register_, const char* key);

/**
 * Try to find variable by name in all variable tables.
 * @param register_ Symbol register
 * @param key variable name to find
 * @return Found variable symbol or NULL
 */
SymbolVariable* symbol_register_find_variable_recursive(SymbolRegister* register_, const char* key);

#endif //_SYMBOL_REGISTER_H
