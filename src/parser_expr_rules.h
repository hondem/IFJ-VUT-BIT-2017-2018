#ifndef _PARSER_EXPR_RULES_H
#define _PARSER_EXPR_RULES_H

#include "parser.h"
#include "parser_expr_internal.h"
#include "llist.h"

// Helper macros

#define EXPR_RULE_CHECK_START() LListItem *tmp, *it = expr_token_buffer->tail
#define EXPR_RULE_CHECK_TYPE(expr_token_type) do {\
    if (it->previous != NULL) { it = it->previous; } else { return false; }\
    if (!(it != NULL && it->value != NULL && ((ExprToken*)it->value)->type == (expr_token_type))) { return false; }\
} while(false)
#define EXPR_RULE_CHECK_FINISH() EXPR_RULE_CHECK_TYPE(EXPR_LEFT_SHARP); tmp = it;
#define EXPR_RULE_NEXT_E_ID() get_next_expr_idx(&tmp)
#define EXPR_RULE_REPLACE(single_expression) expr_replace(expr_token_buffer, it, (single_expression))

// --------------------------

#define EXPR_RULE_TABLE_SIZE 4

typedef bool(*expression_rule_function)(Parser* parser, LList *expr_token_buffer, ExprIdx* expression_idx);
extern const expression_rule_function expr_rule_table[EXPR_RULE_TABLE_SIZE];

// Expression rule headers
bool expression_rule_fake(Parser* parser, LList *expr_token_buffer, ExprIdx* expression_idx);
bool expression_rule_example(Parser* parser, LList *expr_token_buffer, ExprIdx* expression_idx);
// Actual Rules
bool expression_rule_id(Parser* parser, LList *expr_token_buffer, ExprIdx* expression_idx);
bool expression_rule_fn(Parser* parser, LList *expr_token_buffer, ExprIdx* expression_idx);
bool expression_rule_add(Parser* parser, LList *expr_token_buffer, ExprIdx* expression_idx);

#endif //_PARSER_EXPR_RULES_H
