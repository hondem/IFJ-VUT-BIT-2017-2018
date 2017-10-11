#include <stdio.h>
#include "memory.h"
#include "code_instruction.h"
#include "code_instruction_operand.h"
#include "debug.h"
#include "data_type.h"

CodeInstructionOperand* code_instruction_operand_init(TypeInstructionOperand type, CodeInstructionOperandData data) {
    CodeInstructionOperand* operand = memory_alloc(sizeof(CodeInstructionOperand));

    operand->type = type;
    operand->data = data;
    return operand;
}

void code_instruction_operand_free(CodeInstructionOperand** operand_) {
    NULL_POINTER_CHECK(operand_,);
    NULL_POINTER_CHECK(*operand_,);
    // TODO: free specific operands
    CodeInstructionOperand* operand = *operand_;
    switch(operand->type) {
        case TYPE_INSTRUCTION_OPERAND_CONSTANT:
            if(operand->data.constant.data_type == DATA_TYPE_STRING)
                string_free(&(operand->data.constant.data.string));
            break;
        case TYPE_INSTRUCTION_OPERAND_VARIABLE:
            break;
        case TYPE_INSTRUCTION_OPERAND_LABEL:
            memory_free((void*) operand->data.label);
            operand->data.label = NULL;
        default:
            // TODO: free what?
            break;
    }
    memory_free(operand);
    *operand_ = NULL;
}

CodeInstructionOperand* code_instruction_operand_init_variable(SymbolVariable* variable) {
    NULL_POINTER_CHECK(variable, NULL);
    CodeInstructionOperandData data = {.variable=variable};
    return code_instruction_operand_init(TYPE_INSTRUCTION_OPERAND_VARIABLE, data);
}

CodeInstructionOperand* code_instruction_operand_init_integer(int integer) {
    CodeInstructionOperandData data;
    data.constant.data.integer = integer;
    data.constant.data_type = DATA_TYPE_INTEGER;
    return code_instruction_operand_init(TYPE_INSTRUCTION_OPERAND_CONSTANT, data);
}

CodeInstructionOperand* code_instruction_operand_init_double(double double_) {
    CodeInstructionOperandData data;
    data.constant.data.double_ = double_;
    data.constant.data_type = DATA_TYPE_DOUBLE;
    return code_instruction_operand_init(TYPE_INSTRUCTION_OPERAND_CONSTANT, data);
}

CodeInstructionOperand* code_instruction_operand_init_boolean(bool boolean) {
    CodeInstructionOperandData data;
    data.constant.data.boolean = boolean;
    data.constant.data_type = DATA_TYPE_BOOLEAN;
    return code_instruction_operand_init(TYPE_INSTRUCTION_OPERAND_CONSTANT, data);
}

CodeInstructionOperand* code_instruction_operand_init_string(String* string) {
    NULL_POINTER_CHECK(string, NULL);
    CodeInstructionOperandData data;

    data.constant.data.string = string_copy(string);
    data.constant.data_type = DATA_TYPE_STRING;
    return code_instruction_operand_init(TYPE_INSTRUCTION_OPERAND_CONSTANT, data);
}

CodeInstructionOperand* code_instruction_operand_init_label(const char* label) {
    NULL_POINTER_CHECK(label, NULL);
    CodeInstructionOperandData data;
    data.label = c_string_copy(label);
    return code_instruction_operand_init(TYPE_INSTRUCTION_OPERAND_LABEL, data);
}

CodeInstructionOperand* code_instruction_operand_init_data_type(DataType data_type) {
    ASSERT(data_type != DATA_TYPE_NONE);
    CodeInstructionOperandData data;
    data.constant.data_type = data_type;

    return code_instruction_operand_init(TYPE_INSTRUCTION_OPERAND_DATA_TYPE, data);
}


char* code_instruction_operand_render(CodeInstructionOperand* operand) {
    NULL_POINTER_CHECK(operand, NULL);

    size_t length = 1;
    if(operand->type == TYPE_INSTRUCTION_OPERAND_CONSTANT && operand->data.constant.data_type == DATA_TYPE_STRING) {
        length += string_length(operand->data.constant.data.string);
    }

    length += 16; // data type
    char* rendered = memory_alloc(sizeof(char) * length);
    char* escaped = NULL;
    switch(operand->type) {
        case TYPE_INSTRUCTION_OPERAND_LABEL:
            snprintf(rendered, length, "%s", operand->data.label);
            break;
        case TYPE_INSTRUCTION_OPERAND_DATA_TYPE:
            switch(operand->data.constant.data_type) {
                case DATA_TYPE_BOOLEAN:
                    snprintf(rendered, length, "bool");
                    break;
                case DATA_TYPE_DOUBLE:
                    snprintf(rendered, length, "float");
                    break;
                case DATA_TYPE_INTEGER:
                    snprintf(rendered, length, "int");
                    break;
                case DATA_TYPE_STRING:
                    snprintf(rendered, length, "string");
                    break;
                case DATA_TYPE_NONE:
                default:
                    LOG_WARNING("Unknown data type to render: %d.", operand->data.constant.data_type);
            }
            break;
        case TYPE_INSTRUCTION_OPERAND_VARIABLE:
            // TODO: resolve frame
            snprintf(rendered, length, "GF@%s", operand->data.variable->base.key);
            break;
        case TYPE_INSTRUCTION_OPERAND_CONSTANT:
            switch(operand->data.constant.data_type) {
                case DATA_TYPE_INTEGER:
                    snprintf(rendered, length, "int@%d", operand->data.constant.data.integer);
                    break;

                case DATA_TYPE_DOUBLE:
                    snprintf(rendered, length, "float@%g", operand->data.constant.data.double_);
                    break;

                case DATA_TYPE_BOOLEAN:
                    snprintf(rendered, length, "bool@%s", operand->data.constant.data.boolean ? "true" : "false");
                    break;

                case DATA_TYPE_STRING:
                    escaped = code_instruction_operand_escaped_string(operand->data.constant.data.string);
                    snprintf(rendered, length, "string@%s", escaped);
                    memory_free(escaped);
                    break;
                default:
                    LOG_WARNING("Unknown operand data type.");
                    break;
            }
            break;
        default:
            LOG_WARNING("Unknown operand.");
            break;
    }
    return rendered;
}

char* code_instruction_operand_escaped_string(String* source) {
    NULL_POINTER_CHECK(source, NULL);
    size_t source_length = string_length(source);
    String* escaped = string_init_with_capacity((size_t) (source_length * 1.5));
    short c;
    char buffer[5];
    buffer[4] = '\0';
    for(size_t i = 0; i < source_length; ++i) {
        c = source->content[i];
        if((c >= 0 && c <= 32) || c == 35 || c == 92) {
            snprintf(buffer, 4 + 1, "\\%03d", c);
            string_append_s(escaped, buffer);
        } else {
            string_append_c(escaped, (char) c);
        }
    }
    char* escaped_c_string = c_string_copy(string_content(escaped));
    string_free(&escaped);
    return escaped_c_string;
}
