#include "symtable.h"
#include "debug.h"
#include "memory.h"

size_t hash(const char* str);

SymbolTable* symbol_table_init(size_t size, size_t item_size, init_data_callback_f init_data_callback, free_data_callback_f free_data_callback) {
    NULL_POINTER_CHECK(free_data_callback, NULL);
    size_t need_memory = sizeof(SymbolTable) + item_size * size;

    SymbolTable* table = memory_alloc(need_memory);

    table->item_size = item_size;
    table->bucket_count = size;
    table->item_count = 0;
    table->free_data_callback = free_data_callback;
    table->init_data_callback = init_data_callback;
    for(size_t i = 0; i < size; ++i)
        table->items[i] = NULL;

    return table;
}

void symbol_table_free(SymbolTable* table) {
    NULL_POINTER_CHECK(table,);
    symbol_table_clear_buckets(table);

    memory_free(table);
}

size_t symbol_table_size(SymbolTable* table) {
    NULL_POINTER_CHECK(table, 0);
    return table->item_count;
}

size_t symbol_table_bucket_count(SymbolTable* table) {
    NULL_POINTER_CHECK(table, 0);
    return table->bucket_count;
}

void symbol_table_clear_buckets(SymbolTable* table) {
    NULL_POINTER_CHECK(table,);

    SymbolTableBaseListItem* item_to_free = NULL;
    SymbolTableBaseListItem* tmp_item = NULL;

    for(size_t i = 0; i < table->bucket_count; ++i) {
        item_to_free = table->items[i];
        if(item_to_free == NULL) continue;
        do {
            tmp_item = item_to_free;
            item_to_free = item_to_free->next;
            memory_free(tmp_item->key);
            memory_free(tmp_item);
        } while(item_to_free != NULL);
        table->items[i] = NULL;
    }
    table->item_count = 0;
}

SymbolTableBaseListItem* symbol_table_new_item(const char* key, size_t item_size) {
    NULL_POINTER_CHECK(key, NULL);
    SymbolTableBaseListItem* new_item = memory_alloc(item_size);
    char* copied_key = (char*) memory_alloc(sizeof(char) * (strlen(key) + 1));

    if(NULL == strcpy(copied_key, key)) {
        memory_free(new_item);
        memory_free(copied_key);
        return NULL;
    }

    new_item->key = copied_key;
    new_item->next = NULL;

    return new_item;
}

void symbol_table_append_item(SymbolTable* table,
                              SymbolTableBaseListItem* new_item) {
    NULL_POINTER_CHECK(table,);
    NULL_POINTER_CHECK(new_item,);

    size_t index = hash(new_item->key) % table->bucket_count;
    SymbolTableBaseListItem* target = table->items[index];

    new_item->next = NULL;
    if(target == NULL) {
        table->items[index] = new_item;

    } else {
        while(target->next != NULL) {
            target = target->next;
        };
        target->next = new_item;
    }
}


SymbolTableBaseListItem* symbol_table_get(SymbolTable* table, const char* key) {
    NULL_POINTER_CHECK(table, NULL);
    NULL_POINTER_CHECK(key, NULL);

    size_t index = hash(key) % table->bucket_count;
    SymbolTableBaseListItem* item = table->items[index];

    while(item != NULL) {
        if(0 == strcmp(key, item->key))
            return item;
        item = item->next;
    }
    return item;
}

void symbol_table_foreach(SymbolTable* table,
                          void(* callback)(const char*, void*)) {
    NULL_POINTER_CHECK(table,);
    NULL_POINTER_CHECK(callback,);

    for(size_t i = 0; i < table->bucket_count; ++i) {
        SymbolTableBaseListItem* item = (SymbolTableBaseListItem*)table->items[i];
        while(item != NULL) {
            callback(item->key, item);
            item = item->next;
        }
    }
}

SymbolTableBaseListItem* symbol_table_get_or_create(SymbolTable* table, const char* key) {
    NULL_POINTER_CHECK(table, NULL);
    NULL_POINTER_CHECK(key, NULL);

    size_t index = hash(key) % table->bucket_count;
    SymbolTableBaseListItem* item = table->items[index];
    SymbolTableBaseListItem* last_item = NULL;

    while(item != NULL) {
        if(0 == strcmp(key, item->key))
            return item;

        last_item = item;
        item = item->next;
    };

    // key not found, we need to allocate new item
    SymbolTableBaseListItem* new_item = symbol_table_new_item(key, table->item_size);
    if(table->init_data_callback != NULL)
        table->init_data_callback(new_item);

    if(new_item == NULL) return NULL;

    if(last_item == NULL) {
        // not items not found at index
        table->items[index] = new_item;
    } else {
        // link to next
        last_item->next = new_item;
    }
    table->item_count++;

    return new_item;
}

SymbolTable* symbol_table_move(size_t new_size, SymbolTable* source) {
    NULL_POINTER_CHECK(source, NULL);
    SymbolTable* destination = symbol_table_init(new_size, source->item_size,
                                                 source->init_data_callback,
                                                 source->free_data_callback);
    if(destination == NULL) return NULL;

    destination->bucket_count = new_size;

    SymbolTableBaseListItem* item = NULL;
    SymbolTableBaseListItem* next = NULL;

    for(size_t source_bucket_i = 0; source_bucket_i < source->bucket_count; ++source_bucket_i) {
        item = source->items[source_bucket_i];
        source->items[source_bucket_i] = NULL;
        while(item != NULL) {
            next = item->next;
            symbol_table_append_item(destination, item);
            item = next;
        };
    }

    destination->item_count = source->item_count;
    source->item_count = 0;

    return destination;
}

bool symbol_table_remove(SymbolTable* table, const char* key) {
    NULL_POINTER_CHECK(table, false);
    NULL_POINTER_CHECK(key, false);

    size_t index = hash(key) % table->bucket_count;
    SymbolTableBaseListItem* item = table->items[index];
    SymbolTableBaseListItem* prev = NULL;

    while(item != NULL) {
        if(0 == strcmp(key, item->key)) {
            if(prev == NULL)
                table->items[index] = item->next;
            else
                prev->next = item->next;

            memory_free(item->key);
            table->free_data_callback(item);
            memory_free(item);
            return true;
        }

        prev = item;
        item = item->next;
    };

    return false;
}

size_t hash(const char* str) {
    unsigned hash = 5381;
    int c;

    while((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

/** SPECIFIC SYMBOL TABLE IMPLEMENTATIONS */

//SYMBOL_TABLE_TYPED_IMPLEMENTATION(SymbolFunction, function)

//SYMBOL_TABLE_TYPED_IMPLEMENTATION(SymbolVariable, variable)
