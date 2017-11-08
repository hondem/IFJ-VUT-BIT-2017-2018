#include <math.h>
#include "lexer.h"
#include "memory.h"
#include "debug.h"
#include "lexer_fsm.h"


Lexer* lexer_init(lexer_input_stream_f input_stream) {
    Lexer* lexer = (Lexer*) memory_alloc(sizeof(Lexer));
    LexerFSM* lexer_fsm = lexer_fsm_init(input_stream);

    NULL_POINTER_CHECK(input_stream, NULL);

    lexer->lexer_fsm = lexer_fsm;
    lexer->is_token_rewind = false;
    lexer->error_report.error_code = ERROR_NONE;

    return lexer;
}

void lexer_free(Lexer** lexer) {
    NULL_POINTER_CHECK(lexer,);
    NULL_POINTER_CHECK(*lexer,);

    lexer_fsm_free(&((*lexer)->lexer_fsm));
    if ((*lexer)->is_token_rewind) {
        token_free(&(*lexer)->rewind_token);
    }
    memory_free(*lexer);
    *lexer = NULL;
}

void lexer_rewind_token(Lexer* lexer, Token token) {
    NULL_POINTER_CHECK(lexer,);

    ASSERT(!lexer->is_token_rewind);
    lexer->is_token_rewind = true;
    lexer->rewind_token = token_copy(token);
}

void lexer_transform_integer_value(char* integer_value) {

    char* integer_value_copy = memory_alloc(sizeof(integer_value));
    strcpy(integer_value_copy, integer_value);
    int sum = 0;
    int multiplier = 0;

    // First char is type of integer. [0-9] -> decimal, 'b' -> binary, 'o' -> octa, 'h' -> hexa
    switch(integer_value[0]) {


        case 'b':
            // From binary to Decimal
            for(int i = 1; i < strlen(integer_value); i++) {
                if(integer_value[i] == '1')
                    sum = sum + (1 * pow(2, multiplier));
                multiplier = multiplier + 1;
            }

            sprintf(integer_value, "%d", sum);

            break;


        case 'o':
            // From octa to Decimal
            // TODO: Input: something like o12321103221032, tranform it into decimal into input pointer
            break;

        case 'h':
            // From hexa to Decimal
            // TODO: Input: something like h12b2a103221032, tranform it into decimal into input pointer
            break;

    }

    memory_free(integer_value_copy);

}

Token lexer_next_token(Lexer* lexer) {
    Token token = {
            .data = NULL,
            .type = TOKEN_UNKNOWN
    };

    NULL_POINTER_CHECK(lexer, token);

    if(lexer->is_token_rewind) {
        lexer->is_token_rewind = false;

        Token tmp = { .data = lexer->rewind_token.data,.type = lexer->rewind_token.type };
        lexer->rewind_token.data = NULL;
        lexer->rewind_token.type = TOKEN_UNKNOWN;

        return tmp;
    }

    LexerFSMState actual_state = LEX_FSM__INIT;
    do {
        // loop from init state to one of final state
        actual_state = lexer_fsm_next_state(lexer->lexer_fsm, actual_state);
    } while(!lexer_fsm_is_final_state(actual_state));

    token.type = (TokenType) actual_state;

    token.data = lexer_store_token_data(lexer, token);
    string_clear(lexer->lexer_fsm->stream_buffer);

    // Set error information
    if(token.type == TOKEN_ERROR) {
        lexer->error_report.error_code = ERROR_LEXER;
        lexer->error_report.detail_information = (int) lexer->lexer_fsm->lexer_error;
    }

    // Transform integer value to decimal system
    if(token.type == TOKEN_INTEGER_LITERAL) {
        lexer_transform_integer_value(token.data);
    }

    return token;
}

char* lexer_store_token_data(const Lexer* lexer, Token token) {
    NULL_POINTER_CHECK(lexer, NULL);

    size_t data_length = string_length(lexer->lexer_fsm->stream_buffer);
    if(token.type == TOKEN_IDENTIFIER ||
            token.type == TOKEN_STRING_VALUE ||
            token.type == TOKEN_INTEGER_LITERAL ||
            token.type == TOKEN_DOUBLE_LITERAL
            ) {
        char* data = memory_alloc(sizeof(char) * (data_length + 1));

        if (NULL == strcpy(data, string_content(lexer->lexer_fsm->stream_buffer))) {
            exit_with_code(ERROR_INTERNAL);
        }
        return data;
    } else {
        return NULL;
    }
}
