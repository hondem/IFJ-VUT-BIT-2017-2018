#include "code_generator.h"
#include "memory.h"

CodeGenerator* code_generator_init() {
    CodeGenerator* generator = memory_alloc(sizeof(CodeGenerator));

    generator->first = generator->last = NULL;

    return generator;
}

void code_generator_free(CodeGenerator** generator) {
    NULL_POINTER_CHECK(generator,);
    NULL_POINTER_CHECK(*generator,);
    CodeInstruction* instruction = (*generator)->first;
    CodeInstruction* next = NULL;
    while(instruction != NULL) {
        next = instruction->next;
        code_instruction_free(&instruction);
        instruction = next;
    }
}

void code_generator_generate_instruction(
        CodeGenerator* generator,
        TypeInstruction type_instruction,
        CodeInstructionOperand* op0,
        CodeInstructionOperand* op1,
        CodeInstructionOperand* op2
) {
    NULL_POINTER_CHECK(generator,);
    CodeInstruction* instruction = code_instruction_init(type_instruction, op0, op1, op2);

    if(generator->last == NULL) {
        generator->last = generator->first = instruction;
        instruction->prev = instruction->next = NULL;
    } else {
        generator->last->next = instruction;
        instruction->prev = generator->last;
        generator->last = instruction;
    }
}

// TODO: macro generation?
void generate_WRITE(CodeGenerator* generator, CodeInstructionOperand* op0) {
    ASSERT(op0->type & TYPE_INSTRUCTION_OPERAND_SYMBOL);

    code_generator_generate_instruction(generator, I_WRITE, op0, NULL, NULL);
}

void generate_LABEL(CodeGenerator* generator, CodeInstructionOperand* op0) {
    ASSERT(op0->type & TYPE_INSTRUCTION_OPERAND_LABEL);

    code_generator_generate_instruction(generator, I_LABEL, op0, NULL, NULL);
}

void generate_JUMP(CodeGenerator* generator, CodeInstructionOperand* op0) {
    ASSERT(op0->type & TYPE_INSTRUCTION_OPERAND_LABEL);

    code_generator_generate_instruction(generator, I_JUMP, op0, NULL, NULL);
}


void generate_JUMP_IF_EQUAL(CodeGenerator* generator, CodeInstructionOperand* op0, CodeInstructionOperand* op1,
                            CodeInstructionOperand* op2) {
    // TODO: check NULL operands
    ASSERT(op0->type & TYPE_INSTRUCTION_OPERAND_LABEL);
    ASSERT(op1->type & TYPE_INSTRUCTION_OPERAND_SYMBOL);
    ASSERT(op2->type & TYPE_INSTRUCTION_OPERAND_SYMBOL);

    code_generator_generate_instruction(generator, I_JUMP_IF_EQUAL, op0, op1, op2);
}
