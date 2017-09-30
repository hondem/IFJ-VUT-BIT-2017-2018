#include "gtest/gtest.h"

extern "C" {
#include "../src/memory.h"
#include "../src/debug.h"
#include "../src/symtable.h"
}

typedef struct test_structure_t {
    bool foo;
} TestStructure;

SYMBOL_TABLE_TYPED_HEADERS(TestStructure, test_structure);

SYMBOL_TABLE_TYPED_IMPLEMENTATION(TestStructure, test_structure);

class SymbolTableTypedTestFixture : public ::testing::Test {
    protected:
        SymbolTableTestStructure* symbol_table = nullptr;

        virtual void SetUp() {
            symbol_table = symbol_table_test_structure_init(2, FreeData);
        }

        virtual void TearDown() {
            symbol_table_test_structure_free(symbol_table);
        }

        void static FreeData(TestStructure* data) {
        }
};

TEST_F(SymbolTableTypedTestFixture, Finding) {
    EXPECT_EQ(
            symbol_table_test_structure_get(symbol_table, "unknown"),
            nullptr
    ) << "Get on empty typed table.";

    SymbolTableListItemTestStructure* item = symbol_table_test_structure_get_or_create(symbol_table, "unknown");
    SymbolTableListItemTestStructure* found_item;

    EXPECT_NE(
            item,
            nullptr
    ) << "Get on empty typed table.";
    TestStructure* data;
    data = item->data = (TestStructure*) memory_alloc(sizeof(TestStructure));
    item->data->foo = true;

    found_item = symbol_table_test_structure_get(symbol_table, "unknown");
    EXPECT_NE(
            found_item,
            nullptr
    ) << "Successfully found item.";

    found_item->data->foo = false;

    EXPECT_EQ(
            found_item->data->foo,
            item->data->foo
    ) << "Same data on found and previous created item";

    memory_free(data);
}
