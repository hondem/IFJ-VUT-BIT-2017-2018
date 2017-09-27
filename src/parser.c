#include "parser.h"
#include "lexer.h"
#include "lexer_fsm.h"
#include "token.h"


#define GET_NEXT_TOKEN_TYPE() token = lexer_next_token(parser->lexer); token_type = token->type; memory_free(token);

#define INIT_LOCAL_TOKEN_VARS() Token *token; TokenType token_type;

#define CALL_RULE(Rule) if (!parser_parse_##Rule(parser)) return false;

#define TEST_TOKEN_TYPE(Type) if(token_type != Type) return false;


Parser *parser_init(lexer_input_stream_f input_stream) {
    Parser* parser = (Parser*) memory_alloc(sizeof(Parser));

    NULL_POINTER_CHECK(parser, NULL);

    parser->lexer = lexer_init(input_stream);
    return parser;
}

void parser_free(Parser **parser) {

    lexer_free(&((*parser)->lexer));
    memory_free(*parser);
    *parser = NULL;
}

bool parser_parse_program(Parser* parser) {

    INIT_LOCAL_TOKEN_VARS();
    /*
     * RULE
     * <prog> -> <body> EOF
     */

    // Call rule <body>. If <body> return false => return false
    CALL_RULE(body);


    // Expect EOF token If return true, program is syntactically correct
    GET_NEXT_TOKEN_TYPE();
    return (token_type == TOKEN_EOF);

}

bool parser_parse_body(Parser* parser) {

    INIT_LOCAL_TOKEN_VARS()

    /**
     * RULE
     * <body> -> <definitions> SCOPE <statements> END SCOPE
     */

    // Call <definitions> rule
    CALL_RULE(definitions);

    // Expect SCOPE token
    GET_NEXT_TOKEN_TYPE();
    TEST_TOKEN_TYPE(TOKEN_SCOPE);

    // Call <statements> rule
    CALL_RULE(statements);

    // Expect END token
    GET_NEXT_TOKEN_TYPE();
    TEST_TOKEN_TYPE(TOKEN_END);

    // Expect SCOPE token
    GET_NEXT_TOKEN_TYPE();
    return (token_type == TOKEN_SCOPE);
}

bool parser_parse_definitions(Parser* parser) {

    // Todo: It is epsilon, it will be implemented in the future
    return true;
}

bool parser_parse_statements(Parser* parser) {

    // Todo: It is epsilon, it will be implemented in the future
    return true;
}

bool parser_function_param(Parser* parser) {

    INIT_LOCAL_TOKEN_VARS()

    /**
     * RULE
     * <function_param> -> IDENTIFIER AS TYPE
     */

    // Expect IDENTIFIER token
    GET_NEXT_TOKEN_TYPE();
    TEST_TOKEN_TYPE(TOKEN_IDENTIFIER);

    // Expect AS token
    GET_NEXT_TOKEN_TYPE();
    TEST_TOKEN_TYPE(TOKEN_AS);

    // Expect TYPE token
    GET_NEXT_TOKEN_TYPE();
    return (token_type == TOKEN_INTEGER || token_type == TOKEN_STRING || token_type == TOKEN_DOUBLE);

}


