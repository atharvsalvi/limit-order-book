#include <benchmark/benchmark.h>

#include <chrono>
#include <vector>

#include <fstream>
#include <string>
#include <vector>

#include "pipeline/parser.h"
#include "pipeline/simulation.h"
#include "engine/orderbook.h"


// ============================================================
// Helper
// ============================================================

static TimePoint getTime()
{
    return std::chrono::system_clock::now();
}


// ============================================================
// Cleanup
//
// OrderBook owns OrderCard objects.
// Therefore cleanup must use delete.
// ============================================================

static void clearBook()
{
    while (buyHead != nullptr)
    {
        OrderCard* temp = buyHead;
        buyHead = buyHead->next;
        delete temp;
    }

    while (sellHead != nullptr)
    {
        OrderCard* temp = sellHead;
        sellHead = sellHead->next;
        delete temp;
    }
}


// ============================================================
// Benchmark: BUY insertion
// ============================================================

static void BM_AddBuyer(benchmark::State& state)
{
    int orderID = 1;

    for (auto _ : state)
    {
        clearBook();

        OrderCard* buyer = new OrderCard{};

        addBuyer(
            buyer,
            orderID++,
            100.0,
            10,
            getTime()
        );

        benchmark::DoNotOptimize(buyHead);

        clearBook();
    }
}

BENCHMARK(BM_AddBuyer);


// ============================================================
// Benchmark: SELL insertion
// ============================================================

static void BM_AddSeller(benchmark::State& state)
{
    int orderID = 1;

    for (auto _ : state)
    {
        clearBook();

        OrderCard* seller = new OrderCard{};

        addSeller(
            seller,
            orderID++,
            101.0,
            10,
            getTime()
        );

        benchmark::DoNotOptimize(sellHead);

        clearBook();
    }
}

BENCHMARK(BM_AddSeller);


// ============================================================
// Benchmark: Matching
// ============================================================

static void BM_Matching(benchmark::State& state)
{
    int orderID = 1;

    for (auto _ : state)
    {
        clearBook();

        OrderCard* seller = new OrderCard{};
        OrderCard* buyer = new OrderCard{};

        TimePoint now = getTime();

        addSeller(
            seller,
            orderID++,
            100.0,
            10,
            now
        );

        addBuyer(
            buyer,
            orderID++,
            101.0,
            10,
            now
        );

        /*
         * addBuyer() automatically calls matching_engine().
         *
         * Since the BUY price (101) >= SELL price (100),
         * both orders are completely filled and deleted
         * by matching_engine().
         */

        benchmark::ClobberMemory();

        /*
         * Do NOT call clearBook() here.
         * The matching engine already deleted both orders.
         */
    }
}

BENCHMARK(BM_Matching);


// ============================================================
// Benchmark: Partial Fill
// ============================================================

static void BM_PartialFill(benchmark::State& state)
{
    int orderID = 1;

    for (auto _ : state)
    {
        clearBook();

        OrderCard* seller = new OrderCard{};
        OrderCard* buyer = new OrderCard{};

        TimePoint now = getTime();

        // SELL 100 units
        addSeller(
            seller,
            orderID++,
            100.0,
            100,
            now
        );

        // BUY 40 units
        addBuyer(
            buyer,
            orderID++,
            101.0,
            40,
            now
        );

        /*
         * BUY is completely filled.
         *
         * SELL has 60 remaining and therefore remains
         * in the order book.
         */

        benchmark::DoNotOptimize(sellHead);

        clearBook();
    }
}

BENCHMARK(BM_PartialFill);


// ============================================================
// Benchmark: Cancellation
// ============================================================

static void BM_CancelOrder(benchmark::State& state)
{
    int orderID = 1;

    for (auto _ : state)
    {
        clearBook();

        int currentID = orderID++;

        OrderCard* buyer = new OrderCard{};

        addBuyer(
            buyer,
            currentID,
            100.0,
            10,
            getTime()
        );

        cancel_order(currentID);

        benchmark::DoNotOptimize(buyHead);
    }
}

BENCHMARK(BM_CancelOrder);


// ============================================================
// Benchmark: Multiple BUY orders
// ============================================================

static void BM_MultipleBuyOrders(benchmark::State& state)
{
    const int numberOfOrders =
        static_cast<int>(state.range(0));

    for (auto _ : state)
    {
        clearBook();

        TimePoint now = getTime();

        for (int i = 0; i < numberOfOrders; ++i)
        {
            OrderCard* buyer = new OrderCard{};

            addBuyer(
                buyer,
                i + 1,
                100.0 - (i * 0.01),
                10,
                now
            );
        }

        benchmark::DoNotOptimize(buyHead);

        clearBook();
    }

    state.SetItemsProcessed(
        state.iterations() * numberOfOrders
    );
}

BENCHMARK(BM_MultipleBuyOrders)
    ->Arg(100)
    ->Arg(1000)
    ->Arg(10000);


// ============================================================
// Benchmark: Multiple SELL orders
// ============================================================

static void BM_MultipleSellOrders(benchmark::State& state)
{
    const int numberOfOrders =
        static_cast<int>(state.range(0));

    for (auto _ : state)
    {
        clearBook();

        TimePoint now = getTime();

        for (int i = 0; i < numberOfOrders; ++i)
        {
            OrderCard* seller = new OrderCard{};

            addSeller(
                seller,
                i + 1,
                100.0 + (i * 0.01),
                10,
                now
            );
        }

        benchmark::DoNotOptimize(sellHead);

        clearBook();
    }

    state.SetItemsProcessed(
        state.iterations() * numberOfOrders
    );
}

BENCHMARK(BM_MultipleSellOrders)
    ->Arg(100)
    ->Arg(1000)
    ->Arg(10000);


// ============================================================
// Main
// ============================================================

// ============================================================
// Full Simulation Replay
// ============================================================

static void BM_SimulationReplay(benchmark::State& state)
{
    static std::vector<MarketEvent> events;

    if (events.empty())
    {
        const std::string path =
            "src/pipeline/data/simulation_input.csv";

        std::ifstream file(path);

        if (!file.is_open())
        {
            state.SkipWithError(
                "Could not open simulation_input.csv"
            );
            return;
        }

        CSVParser parser;

        std::string line;

        while (std::getline(file, line))
        {
            auto event = parser.parse_line(line);

            if (event.has_value())
            {
                events.push_back(*event);
            }
        }

        if (events.empty())
        {
            state.SkipWithError(
                "No valid events found in CSV"
            );
            return;
        }
    }

    logger.setEnabled(false);

    for (auto _ : state)
    {
        Simulation simulation;

        for (const MarketEvent& event : events)
        {
            simulation.process(event);
        }

        benchmark::DoNotOptimize(buyHead);
        benchmark::DoNotOptimize(sellHead);

        state.PauseTiming();

        clearBook();

        state.ResumeTiming();
    }

    state.SetItemsProcessed(
        state.iterations() *
        static_cast<int64_t>(events.size())
    );

    state.SetLabel(
        std::to_string(events.size()) +
        " events"
    );
}


BENCHMARK(BM_SimulationReplay);


BENCHMARK_MAIN();