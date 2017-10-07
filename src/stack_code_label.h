#ifndef _STACK_CODE_LABEL_H
#define _STACK_CODE_LABEL_H

#include "stack.h"
#include "debug.h"

typedef struct code_label_t {
    StackBaseItem base;

    char* label;
} CodeLabel;

Stack* stack_code_label_init();

void stack_code_label_push(Stack* stack, char* label);

CodeLabel* stack_code_label_pop(Stack* stack);

char* stack_code_label_head(Stack* stack);


#endif //_STACK_CODE_LABEL_H
