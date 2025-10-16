#include <benchmark/benchmark.h>
#include <ql/dummy/dummy.h>

static void BM_Dummy(benchmark::State& st) {
    for  (auto _ : st) {
        ql::Sum(1, 2);
    }
}

BENCHMARK(BM_Dummy);

BENCHMARK_MAIN();
