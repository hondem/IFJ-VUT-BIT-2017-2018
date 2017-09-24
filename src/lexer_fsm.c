#include <stdbool.h>
#include <ctype.h>
#include "lexer_fsm.h"
#include "debug.h"
#include "char_stack.h"
#include "dynamic_string.h"


LexerFSM* lexer_fsm_init(lexer_input_stream_f input_stream) {
    NULL_POINTER_CHECK(input_stream, NULL);
    LexerFSM* lexer = (LexerFSM*) memory_alloc(sizeof(LexerFSM));
    NULL_POINTER_CHECK(lexer, NULL);
    CharStack* stack = char_stack_init();
    lexer->stream_buffer = string_init_with_capacity(LEXER_FSM_STREAM_BUFFER_DEFAULT_LENGHT);
    lexer->stack = stack;
    lexer->input_stream = input_stream;

    return lexer;
}

void lexer_fsm_free(LexerFSM** lexer_fsm) {
    NULL_POINTER_CHECK(lexer_fsm,);
    NULL_POINTER_CHECK(*lexer_fsm,);

    char_stack_free(&(*lexer_fsm)->stack);
    string_free(&(*lexer_fsm)->stream_buffer);
    memory_free(*lexer_fsm);
    *lexer_fsm = NULL;
}

LexerFSMState lexer_fsm_next_state(LexerFSMState prev_state, LexerFSM* lexer_fsm) {
    NULL_POINTER_CHECK(lexer_fsm, LEX_FSM__LEG_SHOT);

    // stored chars in stack from before loops have priority
    int c = char_stack_pop(lexer_fsm->stack);

    if(c == EOF)
        c = lexer_fsm->input_stream();

    switch(prev_state) {

        // Starting state

        case LEX_FSM__INIT:

            // If it is a white space, we ignore it
            if(isspace(c))
                return LEX_FSM__INIT;

            if(c == '_' || isalpha(c)) {
                string_append_c(&(lexer_fsm->stream_buffer), tolower(c));
                return LEX_FSM__IDENTIFIER_UNFINISHED;
            }

            if(isdigit(c)) {
                string_append_c(&(lexer_fsm->stream_buffer), c);
                return LEX_FSM__INTEGER_LITERAL_UNFINISHED;
            }

            switch(c) {
                case '!':
                    return LEX_FSM__STRING_EXC;
                case '\'':
                    return LEX_FSM__COMMENT_LINE;
                case '/':
                    return LEX_FSM__SLASH;
                case '+':
                    return LEX_FSM__ADD;
                case '-':
                    return LEX_FSM__SUBTRACT;
                case '*':
                    return LEX_FSM__MULTIPLY;
                case '(':
                    return LEX_FSM__LEFT_BRACKET;
                case ')':
                    return LEX_FSM__RIGHT_BRACKET;
                case '<':
                    return LEX_FSM__LEFT_SHARP_BRACKET;
                case '>':
                    return LEX_FSM__RIGHT_SHARP_BRACKET;
                case '=':
                    return LEX_FSM__EQUAL;
                case ';':
                    return LEX_FSM__SEMICOLON;
                case ',':
                    return LEX_FSM__COMMA;
                case EOF:
                    return LEX_FSM__EOF;

                default:
                    break;
            }
            break;

            // String states

        case LEX_FSM__STRING_EXC:
            if(c == '"') {
                return LEX_FSM__STRING_LOAD;
            } else {
                return LEX_FSM__LEG_SHOT;
            }

        case LEX_FSM__STRING_LOAD:

            switch(c) {
                case '"':
                    return LEX_FSM__STRING_VALUE;
                case '\n':
                    return LEX_FSM__LEG_SHOT;
                case '\\':
                    return LEX_FSM__STRING_SLASH;
                default:
                    string_append_c(&(lexer_fsm->stream_buffer), c);
                    return LEX_FSM__STRING_LOAD;
            }

        case LEX_FSM__STRING_SLASH:
            switch(c) {
                case '\"':
                    string_append_c(&(lexer_fsm->stream_buffer), '"');
                    return LEX_FSM__STRING_LOAD;
                case '\\':
                    string_append_c(&(lexer_fsm->stream_buffer), '\\');
                    return LEX_FSM__STRING_LOAD;
                case 'n':
                    string_append_c(&(lexer_fsm->stream_buffer), '\n');
                    return LEX_FSM__STRING_LOAD;
                case 't':
                    string_append_c(&(lexer_fsm->stream_buffer), '\t');
                    return LEX_FSM__STRING_LOAD;
                default:
                    return LEX_FSM__LEG_SHOT;
            }

            // Integer literal

        case LEX_FSM__INTEGER_LITERAL_UNFINISHED:
            if(isdigit(c)) {
                string_append_c(&(lexer_fsm->stream_buffer), c);
                return LEX_FSM__INTEGER_LITERAL_UNFINISHED;
            } else if(c == '.') {
                string_append_c(&(lexer_fsm->stream_buffer), c);
                return LEX_FSM__DOUBLE_DOT;
            } else {
                char_stack_push(lexer_fsm->stack, c);
                return LEX_FSM__INTEGER_LITERAL_FINISHED;
            }

            // Double literal

        case LEX_FSM__DOUBLE_DOT:
            if(isdigit(c)) {
                string_append_c(&(lexer_fsm->stream_buffer), c);
                return LEX_FSM__DOUBLE_UNFINISHED;
            } else
                return LEX_FSM__LEG_SHOT;

        case LEX_FSM__DOUBLE_UNFINISHED:
            if(isdigit(c)) {
                string_append_c(&(lexer_fsm->stream_buffer), c);
                return LEX_FSM__DOUBLE_UNFINISHED;
            } else if(tolower(c) == 'e') {
                string_append_c(&(lexer_fsm->stream_buffer), c);
                return LEX_FSM__DOUBLE_E;
            } else {
                char_stack_push(lexer_fsm->stack, c);
                return LEX_FSM__DOUBLE_FINISHED;
            }

        case LEX_FSM__DOUBLE_E:
            if(isdigit(c)) {
                string_append_c(&(lexer_fsm->stream_buffer), c);
                return LEX_FSM__DOUBLE_E_UNFINISHED;
            }
            else if(c == '-' || c == '+') {
                string_append_c(&(lexer_fsm->stream_buffer), c);
                return LEX_FSM__DOUBLE_E_UNFINISHED;
            }
            else {
                return LEX_FSM__LEG_SHOT;
            }

        case LEX_FSM__DOUBLE_E_UNFINISHED:
            if(isdigit(c)) {
                string_append_c(&(lexer_fsm->stream_buffer), c);
                return LEX_FSM__DOUBLE_E_UNFINISHED;
            } else {
                char_stack_push(lexer_fsm->stack, c);
                return LEX_FSM__DOUBLE_FINISHED;

            }

            // Relation operators

        case LEX_FSM__LEFT_SHARP_BRACKET:
            switch(c) {
                case '=':
                    return LEX_FSM__SMALLER_EQUAL;
                case '>':
                    return LEX_FSM__SMALLER_BIGGER;
                default:
                    char_stack_push(lexer_fsm->stack, c);
                    return LEX_FSM__SMALLER;
            }

        case LEX_FSM__RIGHT_SHARP_BRACKET:

            if(c == '=')
                return LEX_FSM__BIGGER_EQUAL;
            else {
                char_stack_push(lexer_fsm->stack, c);
                return LEX_FSM__BIGGER;
            }

            // Identifiers

        case LEX_FSM__IDENTIFIER_UNFINISHED:
            if(c == '_' || isdigit(c) || isalpha(c)) {
                string_append_c(&(lexer_fsm->stream_buffer), tolower(c));
                return LEX_FSM__IDENTIFIER_UNFINISHED;
            } else {
                char_stack_push(lexer_fsm->stack, tolower(c));
                LexerFSMState return_state = lexer_fsm_get_identifier_type(string_content(&(lexer_fsm->stream_buffer)));
                return return_state;
            }

            // Comments

        case LEX_FSM__SLASH:
            if(c == '\'')
                return LEX_FSM__COMMENT_BLOCK;
            else {
                char_stack_push(lexer_fsm->stack, c);
                return LEX_FSM__DIVIDE;
            }


        case LEX_FSM__COMMENT_LINE:
            if(c != '\n')
                return LEX_FSM__COMMENT_LINE;
            return LEX_FSM__INIT;

        case LEX_FSM__COMMENT_BLOCK:
            if(c == '\'')
                return LEX_FSM__COMMENT_BLOCK_END;
            else
                return LEX_FSM__COMMENT_BLOCK;

        case LEX_FSM__COMMENT_BLOCK_END:
            if(c == '/')
                return LEX_FSM__INIT;
            else
                return LEX_FSM__COMMENT_BLOCK;

        default:
            break;
    }

    // TODO never happened, Chuck Norris state
    return LEX_FSM__LEG_SHOT;
}

LexerFSMState lexer_fsm_get_identifier_type(char* name) {
    // TODO: Macro is faster....

    static const int number_of_keywords = 35;

    static const char* keywords[] = {
            // keywords
            "as", "asc", "declare", "dim", "do",
            "double", "else", "end", "chr", "function",
            "if", "input", "integer", "length", "loop",
            "print", "return", "scope", "string", "substr",
            "then", "while",
            // reserved
            "and", "boolean", "continue",
            "elseif", "exit", "false", "for", "next", "not",
            "or", "shared", "static", "true"
    };

    ASSERT(sizeof(keywords) / sizeof(*keywords) == number_of_keywords);

    for(int i = 0; i < number_of_keywords; i++) {
        if(strcmp(keywords[i], name) == 0) {
            return LEX_FSM__AS + i;
        }
    }

    return LEX_FSM__IDENTIFIER_FINISHED;
}

bool lexer_fsm_is_final_state(LexerFSMState state) {
    // TODO: inline of macro to better performance
    if(state >= LEX_FSM__ADD && state <= LEX_FSM__LEG_SHOT)
        return true;
    return false;
}