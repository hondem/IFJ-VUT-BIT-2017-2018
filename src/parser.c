#include "parser.h"
#include "parser_semantic.h"
#include "lexer.h"
#include "lexer_fsm.h"
#include "token.h"


Parser* parser_init(lexer_input_stream_f input_stream) {
    Parser* parser = (Parser*) memory_alloc(sizeof(Parser));

    parser->lexer = lexer_init(input_stream);
    parser->error_report.error_code = ERROR_NONE;
    parser->parser_semantic = parser_semantic_init();
    parser->enabled_code_generation = parser->enabled_semantic_analysis = true;
    return parser;
}

void parser_free(Parser** parser) {

    lexer_free(&((*parser)->lexer));
    memory_free(*parser);
    *parser = NULL;
}

bool parser_parse(Parser* parser) {

    if (!parser_parse_program(parser)) {
        parser->error_report.line = parser->lexer->lexer_fsm->line;

        if (parser->error_report.error_code == ERROR_NONE)
            parser->error_report.error_code = ERROR_SYNTAX;

        return false;
    }

    return true;

}

bool parser_parse_program(Parser* parser) {
    /*
     * RULE
     * <prog> -> <body> <eols> EOF
     */

    // Call rule <body>. If <body> return false => return false
    RULES(
        CHECK_RULE(body);
        CHECK_RULE(eols);

        // Expect EOF token If return true, program is syntactically correct
        CHECK_TOKEN(TOKEN_EOF);
    );

    return true;
}

bool parser_parse_body(Parser* parser) {
    /*
     * RULE
     * <body> -> <definitions> SCOPE EOL <statements> END SCOPE
     */

    RULES(
        CHECK_RULE(definitions);
        CHECK_TOKEN(TOKEN_SCOPE);
        CHECK_TOKEN(TOKEN_EOL);
        CHECK_RULE(body_statements);
        CHECK_TOKEN(TOKEN_END);
        CHECK_TOKEN(TOKEN_SCOPE);
    );

    return true;
}

bool parser_parse_definitions(Parser* parser) {
    /*
     * RULES
     *
     * <definitions> -> <eols> <definition> <definitions>
     * <definitions> -> <eols> E
     */

    RULES(
        CHECK_RULE(eols);
        CONDITIONAL_RULES(
            lexer_rewind_token(parser->lexer, token);
            CHECK_RULE(token_type != TOKEN_DECLARE && token_type != TOKEN_FUNCTION, epsilon, NO_CODE);
            CHECK_RULE(definition);
            CHECK_RULE(definitions);
        );
    );

    return true;
}

bool parser_parse_definition(Parser* parser) {
    /*
     * RULES
     * <definition> -> <function_declaration>
     * <definition> -> <function_definition>
     */

    RULES(
        CONDITIONAL_RULES(
            lexer_rewind_token(parser->lexer, token);
            CHECK_RULE(token_type == TOKEN_DECLARE, function_declaration, NO_CODE);
            CHECK_RULE(token_type == TOKEN_FUNCTION, function_definition, NO_CODE);
            CHECK_RULE(epsilon);
        );
    );

    return true;
}

bool parser_parse_function_definition(Parser* parser) {
    /*
     * RULE
     * <function_definition> -> <function_header> EOL <eols> <statements> END FUNCTION
     */

    RULES(
        CHECK_RULE(function_header);
        CHECK_TOKEN(TOKEN_EOL);
        CHECK_RULE(eols);
        CHECK_RULE(function_statements);
        CHECK_TOKEN(TOKEN_END);
        CHECK_TOKEN(TOKEN_FUNCTION);
    );

    return true;
}

bool parser_parse_function_statements(Parser* parser) {
    /*
     * RULES
     * <statements> -> E
     * <statements> -> <statement_single> EOL <eols> <statements>
     */

    RULES(
        CONDITIONAL_RULES(
            lexer_rewind_token(parser->lexer, token);
            CHECK_RULE(token_type != TOKEN_INPUT, epsilon, NO_CODE);
            CHECK_RULE(function_statement_single);
            CHECK_TOKEN(TOKEN_EOL);
            CHECK_RULE(eols);
            CHECK_RULE(function_statements);
        );
    );

    return true;
}

bool parser_parse_body_statements(Parser* parser) {
    /*
     * RULES
     * <statements> -> E
     * <statements> -> <statement_single> EOL <eols> <statements>
     */

    RULES(
        CONDITIONAL_RULES(
            lexer_rewind_token(parser->lexer, token);
            CHECK_RULE(token_type != TOKEN_INPUT, epsilon, NO_CODE);
            CHECK_RULE(body_statement_single);
            CHECK_TOKEN(TOKEN_EOL);
            CHECK_RULE(eols);
            CHECK_RULE(body_statements);
        );
    );

    return true;
}

bool parser_parse_function_statement_single(Parser* parser) {
    /*
     * RULE
     * <statement_single> -> INPUT <id>
     */

    RULES(
        CONDITIONAL_RULES(
            CHECK_TOKEN(token_type == TOKEN_INPUT, TOKEN_IDENTIFIER, BEFORE(), AFTER(
                return true;
            ));
        );
    );

    return false;
}

bool parser_parse_body_statement_single(Parser* parser) {
    /*
     * RULE
     * <statement_single> -> INPUT <id>
     */

    RULES(
        CONDITIONAL_RULES(
            CHECK_TOKEN(token_type == TOKEN_INPUT, TOKEN_IDENTIFIER, BEFORE(), AFTER(
                SEMANTIC_ANALYSIS(parser,
                    if(NULL == symbol_register_find_variable_recursive(parser->parser_semantic->register_, token->data)) {
                      return false;
                    }
                    return true;
                );
            ));
        );
    );
    return false;
}


bool parser_parse_function_declaration(Parser* parser) {
    /*
     * RULE
     * <function_declaration> -> DECLARE <function_header> EOL <eols>
     */

    RULES(
        CHECK_TOKEN(TOKEN_DECLARE);
        CHECK_RULE(function_header);
        CHECK_TOKEN(TOKEN_EOL);
        CHECK_RULE(eols);
    );

    return true;
}

bool parser_parse_function_header(Parser* parser) {
    /*
     * RULE
     * FUNCTION IDENTIFIER (<function_params>) AS <type>
     */

    RULES(
        CHECK_TOKEN(TOKEN_FUNCTION);
        CHECK_TOKEN(TOKEN_IDENTIFIER);
        CHECK_TOKEN(TOKEN_LEFT_BRACKET);
        CHECK_RULE(function_params);
        CHECK_TOKEN(TOKEN_RIGHT_BRACKET);
        CHECK_TOKEN(TOKEN_AS);
        CHECK_TOKEN(TOKEN_DATA_TYPE_CLASS);

    );

    return true;
}

bool parser_parse_function_params(Parser* parser) {
    /*
     * RULE
     * E
     * <function_param> <function_n_param>
     */

    RULES(
        CONDITIONAL_RULES(
            lexer_rewind_token(parser->lexer, token);
            CHECK_RULE(token_type == TOKEN_RIGHT_BRACKET, epsilon, BEFORE(), AFTER(
                           return true;
            ));
            CHECK_RULE(function_param);
            CHECK_RULE(function_n_param);
        );
    );

    return true;
}

bool parser_parse_function_n_param(Parser* parser) {
    /*
     * RULE
     * E
     * <function_param> <function_n_param>
     */

    RULES(
        CONDITIONAL_RULES(
            CHECK_RULE(token_type == TOKEN_RIGHT_BRACKET, epsilon, BEFORE(), AFTER(
                lexer_rewind_token(parser->lexer, token);
                return true;
            ));
            CHECK_RULE(function_param);
            CHECK_RULE(function_n_param);
        );
    );

    return true;
}

bool parser_parse_function_param(Parser* parser) {
    /*
     * RULE
     * <function_param> -> IDENTIFIER AS TYPE
     */

    RULES(
        CHECK_TOKEN(TOKEN_IDENTIFIER);
        CHECK_TOKEN(TOKEN_AS);
        CHECK_TOKEN(TOKEN_DATA_TYPE_CLASS);
    );

    return true;

}

bool parser_parse_eols(Parser* parser) {
    /*
     * RULES
     * <eols> -> E
     * <eols> -> EOL <eols>
     */

    RULES(
        CONDITIONAL_RULES(
            CHECK_RULE(token_type == TOKEN_EOL, eols, NO_CODE);
            CHECK_RULE(epsilon, BEFORE(), AFTER(
                           lexer_rewind_token(parser->lexer, token);));
        );
    );

    return true;
}

bool parser_parse_epsilon(Parser* parser)
{
    UNUSED(parser);

    return true;
}
