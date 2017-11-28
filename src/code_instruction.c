#include "code_instruction.h"
#include "memory.h"

CodeInstruction* code_instruction_init(
        TypeInstruction type,
        CodeInstructionOperand* op0,
        CodeInstructionOperand* op1,
        CodeInstructionOperand* op2
) {
    CodeInstruction* instruction = memory_alloc(sizeof(CodeInstruction));
    instruction->type = type;
    instruction->op0 = op0;
    instruction->op1 = op1;
    instruction->op2 = op2;
    instruction->meta_data.type = CODE_INSTRUCTION_META_TYPE_NONE;
    instruction->meta_data.purity_type = META_TYPE_PURE;

    instruction->prev = instruction->next = NULL;

    return instruction;
}

void code_instruction_free(CodeInstruction** instruction) {
    NULL_POINTER_CHECK(instruction,);
    NULL_POINTER_CHECK(*instruction,);

    if((*instruction)->op0 != NULL) code_instruction_operand_free(&(*instruction)->op0);
    if((*instruction)->op1 != NULL) code_instruction_operand_free(&(*instruction)->op1);
    if((*instruction)->op2 != NULL) code_instruction_operand_free(&(*instruction)->op2);
    memory_free(*instruction);
    *instruction = NULL;
}

char* code_instruction_render(CodeInstruction* instruction) {
    NULL_POINTER_CHECK(instruction, NULL);
    NULL_POINTER_CHECK(instruction->signature_buffer, NULL);
    NULL_POINTER_CHECK(instruction->signature_buffer->identifier, NULL);
    char* op0 = NULL;
    char* op1 = NULL;
    char* op2 = NULL;
    size_t length = 1;
    if(instruction->op0) {
        op0 = code_instruction_operand_render(instruction->op0);
        length += strlen(op0);
    }
    if(instruction->op1) {
        op1 = code_instruction_operand_render(instruction->op1);
        length += strlen(op1);
    }
    if(instruction->op2) {
        op2 = code_instruction_operand_render(instruction->op2);
        length += strlen(op2);
    }

    length += strlen(instruction->signature_buffer->identifier) + 3; // three spaces
    char* formatted = (char*) memory_alloc(sizeof(char) * length);
    snprintf(
            formatted,
            length,
            "%s %s %s %s",
            instruction->signature_buffer->identifier,
            op0 == NULL ? "" : op0,
            op1 == NULL ? "" : op1,
            op2 == NULL ? "" : op2
    );
    if(op0 != NULL)
        memory_free(op0);
    if(op1 != NULL)
        memory_free(op1);
    if(op2 != NULL)
        memory_free(op2);
    return formatted;
}

TypeInstructionClass instruction_class(CodeInstruction* instruction)
{
    if(instruction == NULL)
        return INSTRUCTION_TYPE_INVALID;

    switch (instruction->type) {
        case I_READ:
        case I_MOVE:
        case I_POP_STACK:
            return INSTRUCTION_TYPE_WRITE;

        case I_JUMP:
        case I_CALL:
            return INSTRUCTION_TYPE_DIRECT_JUMP;

        case I_JUMP_IF_EQUAL:
        case I_JUMP_IF_EQUAL_STACK:
        case I_JUMP_IF_NOT_EQUAL:
        case I_JUMP_IF_NOT_EQUAL_STACK:
            return INSTRUCTION_TYPE_CONDITIONAL_JUMP;

        default:
            return INSTRUCTION_TYPE_OTHER;
    }
}
