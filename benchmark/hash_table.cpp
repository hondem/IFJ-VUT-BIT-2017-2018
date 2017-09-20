#include <benchmark/benchmark.h>
#include <cmath>
#include "../src/ial.c"

class HashTableFixture : public benchmark::Fixture {
    protected:
        HashTable* table;
};

void _none_callback(const char* key, void* data) {

}

BENCHMARK_DEFINE_F(HashTableFixture, Foreach)(benchmark::State &st) {
    table = hash_table_init(static_cast<size_t>(st.range(0)));
    for (int i = 0; i < st.range(1); ++i) {
        hash_table_get_or_create(table, std::to_string(i).c_str());
    }

    while (st.KeepRunning()) {
        hash_table_foreach(table, _none_callback);
    }
}

// Register the function as a benchmark
BENCHMARK_REGISTER_F(HashTableFixture, Foreach)->RangeMultiplier(2)->Ranges({{1, 4},
                                                                             {8, 32}});
