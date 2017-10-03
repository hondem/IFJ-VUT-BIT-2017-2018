#include <utility>

#include "gtest/gtest.h"
#include "utils/stringbycharprovider.h"


extern "C" {
#include "../src/lexer.h"
#include "../src/parser.h"
#include "../src/memory.h"
}

class ParserSemanticTestFixture : public ::testing::Test {
    protected:
        Parser* parser;
        StringByCharProvider* provider;

        void SetUp() override {
            parser = parser_init(token_stream);
            parser->run_type = PARSER_RUN_TYPE_SEMANTIC_ANALYSIS;
            provider = StringByCharProvider::instance();
        }

        void TearDown() override {
            parser_free(&parser);
        }
};

TEST_F(ParserSemanticTestFixture, FunctionStatementSingle) {
    symbol_register_push_variables_table(parser->parser_semantic->register_);
    symbol_table_get_or_create(parser->parser_semantic->register_->variables->symbol_table, "ahoj");

    provider->setString("input ahoj");

    EXPECT_TRUE(
            parser_parse_function_statement_single(parser)
    ) << "Error parsing <definitions> rule";

}
