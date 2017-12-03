#pragma once
#ifndef _LLIST_H
#define _LLIST_H

#include "debug.h"
#include "memory.h"

/**
 * @brief The LListItem structure is single item of linear list which contains single integer
 * value.
 */
typedef struct LListBaseItem {
    struct LListBaseItem* next;
    struct LListBaseItem* previous;
} LListBaseItem;

/**
* @brief Function to free LListItem data
*/
typedef void (* llist_free_item_data_callback_f)(LListBaseItem*);

/**
* @brief Function to initialize LListItem data
*/
typedef void (* llist_init_item_data_callback_f)(LListBaseItem*);

/**
* @brief Comparison function for comparing LListItem datas
* @return -1 if a < b, 0 if a == b, 1 if a > b
*/
typedef int (* llist_item_compare_function)(LListBaseItem* a, LListBaseItem* b);

/**
 * @brief The LList structure contains the beginning and the end of linear list. An empty list
 * has null end and beginning.
 */
typedef struct LList {
    size_t item_size;
    /// The beginning of list.
    LListBaseItem* head;
    /// The end of list.
    LListBaseItem* tail;
    /// Function used to free item.
    llist_free_item_data_callback_f free_data_callback;
    llist_init_item_data_callback_f init_data_callback;
    llist_item_compare_function cmp_function;
} LList;

/**
 * @brief llist_init Allocates memory for list and null head and tail.
 * @param list Address of pointer which will point to llist structure.
 */
void llist_init(LList** list, size_t item_size, llist_init_item_data_callback_f init_function,
                llist_free_item_data_callback_f free_function, llist_item_compare_function cmp_function);

/**
 * @brief llist_init Use llist on stack.
 * @param list Address of pointer which will point to llist structure.
 */
void llist_init_list(LList* list, size_t item_size, llist_init_item_data_callback_f init_function,
                     llist_free_item_data_callback_f free_function, llist_item_compare_function cmp_function);

/**
 * @brief llist_append Appends one item with given value to the end of the list.
 * @param list List where item will be appended.
 * @param value Value of appended item.
 */
LListBaseItem* llist_new_tail_item(LList* list);

/**
 * Append item to list.
 * @param list instance
 * @param new_item new item to append
 * @return appended item
 */
LListBaseItem* llist_append_item(LList* list, LListBaseItem* new_item);

/**
* @brief llist_pop_back Pops item from the end of the list.
* @param list List from which the item will be popped.
* @return Value of popped item.
*/
LListBaseItem* llist_pop_back(LList* list);

/**
* @brief llist_append Inserts one item with given value after item 'after' of the list.
* @param list List where item will be inserted.
* @param after Item from given list after which the item will be inserted.
* @param value Value of inserted item.
*/
void llist_insert_after(LList* list, LListBaseItem* after, LListBaseItem* new_item);

/**
* @brief llist_remove_item Removes one item from the list.
* @param list List from which the item will be removed.
* @param item Item which will be removed.
* @return Next item after removed one.
*/
LListBaseItem* llist_remove_item(LList* list, LListBaseItem* item);

/**
 * @brief llist_free  all items and list and set pointer to null.
 * @param list List which will be d.
 */
void llist_free(LList** list);

/**
 * @brief llist_length Return items count in list.
 * @param list List whose items will be counted.
 * @return Items count in list.
 */
size_t llist_length(LList* list);

/**
 * Get n-th item from end of given list.
 * @param list instance
 * @param n index, 0 is first
 * @return NULL if not found, else found item
 */
LListBaseItem* llist_get_n_from_end(LList* list, size_t n);

#endif //_LLIST_H
