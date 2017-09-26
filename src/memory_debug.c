#include "memory.h"
#include "error.h"

#ifndef NDEBUG

MemoryManager memory_manager;

#define INFO_MAX_LENGTH 128
#define INFO_FORMAT "%s:%d:%s()"

void* memory_manager_malloc(
        size_t size,
        const char* file,
        unsigned line,
        const char* func,
        MemoryManager* manager
) {
    if(!size) {
        LOG_WARNING("Invalid size %zd.", size);
        return NULL;
    }
    if(manager == NULL)
        manager = &memory_manager;

    MemoryManagerPage* new_page = (MemoryManagerPage*) malloc(sizeof(MemoryManagerPage));
    NULL_POINTER_CHECK(new_page, NULL);

    new_page->size = size;
    new_page->allocated = true;
    new_page->info = (char*) malloc(MEMORY_MANAGER_INFO_MAX_LENGTH + 1);
    MALLOC_CHECK(new_page->info);
    new_page->address = malloc(new_page->size);
    if(new_page->address == NULL) {
        // at first free allocated info
        free(new_page->info);
        MALLOC_CHECK(new_page->address);
    }

    snprintf(new_page->info, MEMORY_MANAGER_INFO_MAX_LENGTH, MEMORY_MANAGER_INFO_FORMAT, file, line, func);

    new_page->next = manager->head;
    manager->head = new_page;
    LOG_DEBUG("Successfully allocated %zu bytes for %s.", new_page->size, new_page->info);
    return new_page->address;
}

void memory_manager_free(void* address,
                         MemoryManager* manager) {
    NULL_POINTER_CHECK(address,);
    if(manager == NULL)
        manager = &memory_manager;

    MemoryManagerPage* page = manager->head;
    while((page != NULL) && (page->address != address)) {
        page = page->next;
    }
    if(page == NULL) {
        LOG_WARNING("Allocated memory with address %p to free not found.", address);
        return;
    }
    page->allocated = false;
    free(page->address);
    page->address = NULL;

}

void memory_manager_enter(MemoryManager* manager) {
    LOG_DEBUG("Memory manager started.");
    if(manager == NULL)
        manager = &memory_manager;
    if(manager->head != NULL) {
        LOG_WARNING("Try to enter already entered memory manager session.");
        return;
    }
    manager->head = NULL;
}

void memory_manager_exit(MemoryManager* manager) {
    if(manager == NULL)
        manager = &memory_manager;
    MemoryManagerPage* page = manager->head;
    MemoryManagerPage* next = NULL;
    int pages_count = 0;
    int size_sum = 0;

    while(page != NULL) {
        next = page->next;
        pages_count++;
        size_sum += page->size;

        if(page->allocated) {
            LOG_WARNING("Memory leak of %zu bytes from %s.", page->size, page->info);
            free(page->address);
        }
        free(page->info);
        free(page);
        page = next;
    }

    LOG_DEBUG(
            "Memory manager exited, %d pages freed of total size %d bytes.",
            pages_count,
            size_sum
    );
    manager->head = NULL;
}

void memory_manager_log_stats(MemoryManager* manager) {
    if(manager == NULL)
        manager = &memory_manager;

    MemoryManagerPage* page = manager->head;
    MemoryManagerPage* next = NULL;
    int allocated_pages_count = 0;
    int allocated_size = 0;
    int total_size = 0;

    while(page != NULL) {
        next = page->next;

        allocated_size += page->size * page->allocated;
        allocated_pages_count += page->allocated;
        total_size += page->size;

        page = next;
    }
    LOG_DEBUG(
            "Allocated %d bytes in %d pages. Total memory usage %d bytes.",
            allocated_size, allocated_pages_count, total_size
    );
}

#endif
