#ifndef _CHAR_STACK_H
#define _CHAR_STACK_H

#include <stdlib.h>
#include "memory.h"

#define CHAR_STACK_MAX_LENGTH 256

typedef struct char_stack_t {
    ssize_t stack_head;
    char stack[CHAR_STACK_MAX_LENGTH];
} CharStack;


CharStack* char_stack_init() {
    CharStack* stack = (CharStack*) memory_alloc(sizeof(CharStack));
    NULL_POINTER_CHECK(stack, NULL);
    stack->stack_head = -1;
    return stack;
}

void char_stack_free(CharStack** stack) {
    NULL_POINTER_CHECK(stack,);
    NULL_POINTER_CHECK(*stack,);

    memory_free(*stack);
    *stack = NULL;
}

bool char_stack_push(CharStack* stack, char c) {
    NULL_POINTER_CHECK(stack, false);
    if (stack->stack_head + 1 >= CHAR_STACK_MAX_LENGTH) return false;

    stack->stack[++stack->stack_head] = c;
    return true;
}

int char_stack_pop(CharStack* stack) {
    NULL_POINTER_CHECK(stack, EOF);
    return stack->stack_head >= 0 ? stack->stack[stack->stack_head--] : EOF;
}

#endif //_CHAR_STACK_H
