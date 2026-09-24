# Limit Order Book

A C++20 implementation of a **Limit Order Book (LOB) and matching engine** built from scratch to explore market microstructure, price-time priority, order matching, trade generation, and real-time order-book visualization.

The project supports a CSV-driven order-flow simulation, limit-order matching, order cancellation, trade logging, and an interactive terminal-based order-book interface.

---

## Overview

A Limit Order Book maintains outstanding buy and sell orders for a financial instrument.

The book is divided into two sides:

* **Bids** — orders willing to buy
* **Asks** — orders willing to sell

Orders follow **price-time priority**:

1. Better price gets priority.
2. At the same price, the earlier order gets priority.

For example, the bid side is arranged from the highest price to the lowest:

```text
BUY

₹316.30
₹311.85
₹288.50
₹271.85
```

While the ask side is arranged from the lowest price to the highest:

```text
SELL

₹294.35
₹301.70
₹308.50
```

When the best bid is greater than or equal to the best ask, the matching engine executes a trade.

---

## Features

### Core Matching Engine

* Buy and sell limit-order insertion
* Price-time priority
* FIFO ordering for orders at the same price
* Automatic order matching
* Partial fills
* Order cancellation
* Trade generation
* Best bid and best ask tracking

### Order Book Analytics

The system provides:

* Best Bid
* Best Ask
* Bid-Ask Spread
* Mid Price
* Ask Depth
* Bid Depth
* Aggregated quantity at each price level

The mid-price is calculated as:

```text
Mid Price = (Best Bid + Best Ask) / 2
```

The bid-ask spread is:

```text
Spread = Best Ask - Best Bid
```

**Bid depth** → sum of all resting quantities on the **bid side**.

```text
Bids:
100 → 20
99  → 30

Bid Depth = 20 + 30 = 50
```

**Ask depth** → sum of all resting quantities on the **ask side**.

```text
Asks:
101 → 15
102 → 25

Ask Depth = 15 + 25 = 40
```

---

## Terminal User Interface

The project uses **FTXUI** to provide a real-time terminal interface for monitoring the order book.

The interface displays:

* Ask-side depth
* Bid-side depth
* Price levels
* Quantities
* Best bid
* Best ask
* Spread
* Mid price
* Ask depth
* Bid depth
* Recent trades

A simplified representation of the interface:

```text
┌─────────────────────────────────────────────────────┐
│                  LIMIT ORDER BOOK                   │
├────────────────────────┬────────────────────────────┤
│         ASKS           │           BIDS             │
│                        │                            │
│ QTY          PRICE     │ PRICE          QTY         │
│ ...          ...       │ ...            ...         │
│                        │                            │
├────────────────────────┴────────────────────────────┤
│ Best Bid  : ...       Best Ask : ..                 │
│ Spread    : ...       Mid Price: ...                |
| Ask Depth : ...       Bid Depth: ...                |
├─────────────────────────────────────────────────────┤
│                    RECENT TRADES                    │
│ PRICE       SIDE       QTY                          │
│ ...         ...        ...                          │
└─────────────────────────────────────────────────────┘
```

Press **`Q`** to exit the interface.

---

## Architecture

The project is organized into several components:

```text
limit-order-book/
│
├── CMakeLists.txt
├── README.md
├── .gitignore
│
└── src/
    │
    ├── main.cpp
    │
    ├── order.h
    ├── orderbook.h
    ├── orderbook.cpp
    │
    ├── tradelog.h
    ├── tradelog.cpp
    │
    ├── dashboard.h
    ├── dashboard.cpp
    │
    ├── tui.h
    ├── tui.cpp
    │
    ├── publisher.h
    ├── publisher.cpp
    │
    ├── logger/
    │   └── logger.h
    │
    └── pipeline/
        ├── parser.h
        ├── parser.cpp
        ├── simulation_input.csv
        ├── RELIANCE.csv
        ├── RELIANCE1.csv
        └── generator.ipynb
```

---

## Order Representation

Each order is represented by an `OrderCard` containing information such as:

| Field        | Description                              |
| ------------ | ---------------------------------------- |
| `orderID`    | Unique order identifier                  |
| `price`      | Limit price                              |
| `quantity`   | Remaining quantity                       |
| `arriveTime` | Time at which the order entered the book |
| `prev`       | Previous order in the linked list        |
| `next`       | Next order in the linked list            |

The `prev` and `next` pointers allow orders to form an **intrusive doubly linked list**.

---

## Order Book Data Structure

The current implementation maintains separate linked lists for the buy and sell sides.

```text
buyHead  → highest-priced BUY order
sellHead → lowest-priced SELL order
```

### Buy Side

Orders are maintained in descending price order:

```text
BUY

₹300 → ₹295 → ₹290 → ₹285
 ▲
 buyHead
```

### Sell Side

Orders are maintained in ascending price order:

```text
SELL

₹305 → ₹310 → ₹315 → ₹320
 ▲
sellHead
```

Orders at the same price maintain their arrival order, providing FIFO time priority.

Because the lists are ordered by price, the head of each list represents the current best price.

---

## Matching Engine

The matching engine checks whether the best bid can trade with the best ask:

```text
Best Bid >= Best Ask
```

When this condition is satisfied, a trade is executed.

The trade quantity is:

```text
Trade Quantity = min(Bid Quantity, Ask Quantity)
```

After execution, the quantities of both orders are reduced.

If an order is completely filled, it is removed from the order book.

This allows the engine to naturally handle **partial fills**.

### Example

Suppose the book contains:

```text
Best Bid:  ₹100 × 500
Best Ask:  ₹99  × 300
```

Since:

```text
₹100 >= ₹99
```

the two orders can match.

The executed quantity is:

```text
min(500, 300) = 300
```

The resulting book becomes:

```text
Best Bid: ₹100 × 200
Best Ask: removed
```

---

## Execution Price

When two orders match, the execution price is determined using the arrival time of the orders.

The order that was already resting in the book provides the execution price, while the incoming order consumes that liquidity.

This follows the fundamental behavior of a price-time-priority limit order book.

---

## Trade Log

Every executed trade is recorded in the trade log.

Each trade contains:

| Field         | Description               |
| ------------- | ------------------------- |
| `price`       | Execution price           |
| `side`        | Side of the trade         |
| `quantity`    | Executed quantity         |
| `executeTime` | Trade execution timestamp |

The recent trade history is displayed directly in the TUI.

---

## Simulation Pipeline

The order book can replay a sequence of events from:

```text
src/pipeline/simulation_input.csv
```

The supported commands are:

### Add Order

```text
ADD,<id>,<side>,<quantity>,<price>
```

Example:

```text
ADD,1,SELL,4456424,251.70
ADD,2,BUY,9487878,271.85
```

### Cancel Order

```text
CANCEL,<id>
```

Example:

```text
CANCEL,1
```

The parser reads each event and applies the corresponding operation to the live order book.

The simulation currently introduces a delay between events so that order-book changes can be observed in real time through the TUI.

---

## Simulation Data

The repository contains historical data and a generator notebook:

```text
src/pipeline/
├── RELIANCE.csv
├── RELIANCE1.csv
└── generator.ipynb
```

The generator notebook can be used to create synthetic order-flow events from historical market data.

The generated events contain:

* Order IDs
* Buy/Sell side
* Quantities
* Prices
* Cancellation events

These events are written into the simulation input used by the matching engine.

---

## Order Book Dashboard

The dashboard aggregates individual orders at the same price level.

For example, if the book contains:

```text
BUY ₹100 × 50
BUY ₹100 × 25
BUY ₹99  × 40
```

the dashboard represents the book as:

```text
PRICE     QTY
₹100      75
₹99       40
```

This provides a price-level view of available liquidity.

The dashboard also calculates:

```text
Best Bid
Best Ask
Spread
Mid Price
```

---

## Project Components

### `orderbook.cpp`

Contains the core order-book operations, including:

* Adding buy orders
* Adding sell orders
* Matching orders
* Removing orders
* Cancelling orders

This is the central component of the matching engine.

### `tradelog.cpp`

Maintains and displays executed trades.

### `dashboard.cpp`

Provides a read-only representation of the current order book and calculates key market statistics.

### `tui.cpp`

Implements the terminal interface using FTXUI and displays the current state of the order book and recent trades.

### `pipeline/parser.cpp`

Reads CSV simulation events and feeds them into the matching engine.

### `logger/logger.h`

Provides functionality related to recording order information for later replay and analysis.

### `publisher.cpp`

Contains the infrastructure for preparing order-book snapshots for external publishing.

---

## Testing

The project includes a **GoogleTest** test suite for the core order-book and matching-engine functionality.

The tests cover:

- Buy and sell order insertion
- Price-time priority
- FIFO ordering at the same price
- Full fills
- Partial fills
- Multiple fills
- Price-crossing and non-crossing orders
- Equal-price matching
- Order cancellation
- Cancellation of head, middle, and tail orders
- Cancellation of nonexistent orders
- Linked-list integrity
- Cancellation of already-filled orders
- Execution price based on the resting order
- Aggressor-side trade classification

### Running the Tests

From the project root, configure the build:

```bash
cmake -S . -B build
```

Build the project and tests:

```bash
cmake --build build
```

Run the test suite:

```bash
ctest --test-dir build --output-on-failure
```

A successful test run confirms that the core order-book and matching-engine behavior passes the implemented unit tests.

---

## Build Requirements

The project requires:

* **C++20-compatible compiler**
* **CMake 3.14 or newer**

The project uses:

* **FTXUI 6.1.9** for the terminal interface
* **Google Benchmark 1.8.3** for benchmarking infrastructure
* **Google Test 1.17.0** for for testing

Dependencies are configured through CMake.

---

## Build

Clone the repository:

```bash
git clone https://github.com/atharvsalvi/limit-order-book.git
cd limit-order-book
```

Create a build directory:

```bash
mkdir build
cd build
```

Configure the project:

```bash
cmake ..
```

Build:

```bash
cmake --build .
```

The main executable is:

```text
quant_run
```

---

## Run

From the `build` directory:

### Linux / macOS

```bash
./quant_run
```

### Windows

```bash
quant_run.exe
```

The application will start the simulation and display the live order book through the terminal interface.

---

## Execution Flow

The overall application flow is:

```text
                simulation_input.csv
                        │
                        ▼
                   CSV Parser
                        │
                ┌───────┴───────┐
                │               │
               ADD            CANCEL
                │               │
                ▼               ▼
           Order Book      Cancel Order
                │
                ▼
          Matching Engine
                │
        ┌───────┴────────┐
        │                │
        ▼                ▼
    Order Book        Trade Log
        │                │
        ▼                ▼
    Dashboard        Recent Trades
        │                │
        └───────┬────────┘
                ▼
               TUI
```

The matching engine is the central component, while the dashboard and TUI provide a real-time representation of its state.

---

## Performance Benchmarking and Validation

The project now includes a **Google Benchmark** target for measuring the performance of the order book and simulation pipeline.

### Benchmarking Goals

Benchmarking is used to answer two separate questions:

1. **How fast is the system?** — measured using Google Benchmark.
2. **Where is the CPU time being spent?** — investigated using Linux `perf`.

The workflow used for performance validation is:

```text
Correctness Tests
       │
       ▼
Google Benchmark
       │
       ▼
CPU Profiling with perf
       │
       ▼
Identify Bottlenecks
       │
       ▼
Isolate Components
       │
       ▼
Benchmark Again
```

### Benchmark Target

The benchmark executable is:

```text
build/orderbook_bench
```

The benchmark source is:

```text
benchmarks/orderbook_benchmark.cpp
```

The benchmark target links against Google Benchmark and the core order-book implementation.

### Available Benchmarks

The benchmark suite includes measurements for operations such as:

- `BM_AddBuyer`
- `BM_AddSeller`
- `BM_Matching`
- `BM_PartialFill`
- `BM_CancelOrder`
- `Multiple-order insertion`
- `BM_SimulationReplay`

The simulation replay benchmark processes a fixed input of **5,306 market events** per benchmark iteration.

### Running the Benchmarks

Run all benchmarks:

```bash
./build/orderbook_bench
```

Run only the simulation replay benchmark:

```bash
./build/orderbook_bench \
    --benchmark_filter=BM_SimulationReplay
```

Run the simulation replay benchmark with repeated measurements:

```bash
./build/orderbook_bench \
    --benchmark_filter=BM_SimulationReplay \
    --benchmark_repetitions=10
```

The repeated benchmark is useful because individual measurements can vary due to CPU frequency scaling and other system activity.

### Benchmark Metrics

Google Benchmark reports several useful values.

#### Time

The elapsed time required for one benchmark iteration.

```text
ns = nanoseconds
```

For example:

```text
533559 ns ≈ 0.534 ms
```

#### CPU

The amount of CPU time consumed by the benchmark iteration.

For CPU-bound work, CPU time and elapsed time are often close.

#### Iterations

The number of times Google Benchmark executes the benchmark during a measurement.

Google Benchmark chooses this automatically to obtain sufficiently stable measurements.

#### Items per Second

The throughput of the benchmark.

For this project:

```text
items_per_second
```

represents **market events processed per second**.

For example:

```text
10.0361 M/s
```

means approximately:

```text
10.04 million market events per second
```

#### Mean

The arithmetic average across repeated benchmark runs.

#### Median

The middle measurement after sorting the repeated runs. It is useful when a few unusually slow runs affect the mean.

#### Standard Deviation

Measures how much the repeated measurements vary around the mean.

#### Coefficient of Variation

The coefficient of variation is:

```text
CV = (standard deviation / mean) × 100
```

It gives a relative measure of benchmark variability.

### Initial Simulation Replay Baseline

The first 10-run simulation replay benchmark was measured with the normal logging and mutex paths enabled.

The observed results were approximately:

```text
Mean time:        1,112,303 ns
Mean throughput:  4.78695 M events/sec
Median throughput:4.92283 M events/sec
CV:               6.46%
Events/replay:    5,306
```

This represents the initial performance baseline for the simulation replay workload.

---

## CPU Profiling with Linux `perf`

Linux `perf` is used to determine **where CPU time is being spent** rather than only measuring total execution time.

A profiling run can be started with:

```bash
perf record -g \
./build/orderbook_bench \
--benchmark_filter=BM_SimulationReplay
```

The collected samples can then be inspected with:

```bash
perf report
```

On systems where performance monitoring is restricted, the kernel setting may need to be adjusted for the current session:

```bash
sudo sysctl -w kernel.perf_event_paranoid=1
```

The profiling environment may still report warnings about restricted kernel symbols. These warnings do not prevent user-space application profiling when the required performance events are available.

### How `perf` Sampling Works

`perf` periodically samples the program while it is running and records which functions are executing.

For example, if a function appears in a large percentage of samples, that function or its call tree is consuming a significant portion of CPU activity under the measured workload.

The `perf report` output includes:

- **Self** — CPU samples directly attributed to the function.
- **Children** — samples attributed to the function and work performed by functions it calls.

Therefore, `Children` should not be interpreted as the function's own direct CPU cost.

### Initial Profiling Findings

The initial profile of the simulation replay with logging and mutexes enabled showed significant activity in C++ stream/file-output functions, including:

```text
std::basic_ofstream
std::ostream
std::__ostream_insert
std::ostream::flush
```

This corresponds to the order logger performing operations such as:

```cpp
logFile << ...;
logFile.flush();
```

The profile also showed measurable activity in:

```text
cancel_order()
addSeller()
matching_engine()
addBuyer()
operator new
malloc
free
pthread_mutex_lock
pthread_mutex_unlock
```

Representative `perf` results from this workload included approximately:

```text
cancel_order()
    Self:     26.63%
    Children: 33.68%

addSeller()
    Self:      7.96%
    Children: 20.98%

matching_engine()
    Self:      5.34%
    Children: 19.51%

addBuyer()
    Self:      2.54%
    Children: 14.21%
```

The report also showed a few percent of sampled activity associated with mutex locking/unlocking and dynamic memory allocation.

These percentages are **workload-specific profiling observations**, not universal performance characteristics of the functions.

---

## Isolating Logger and Mutex Overhead

The initial profile indicated that logging and synchronization could be contributing substantial overhead. Instead of immediately modifying the core data structure, the benchmark was changed to isolate these costs.

The benchmark configuration can disable logging and mutex locking while leaving the normal application configuration unchanged.

### Logger

The `OrderLogger` supports an enabled/disabled mode:

```cpp
logger.setEnabled(false);
```

When disabled, `logOrder()` returns without performing file output.

Normal application behavior keeps logging enabled.

### Mutexes

The benchmark uses the compile-time definition:

```text
LOB_BENCHMARK_NO_LOCKS
```

When this definition is enabled for the benchmark target, the benchmark does not incur the `bookMutex` and `tradeMutex` locking overhead.

The normal application does not use this benchmark definition and therefore retains its synchronization behavior.

This separation is important because disabling logging and mutexes is a **benchmarking experiment**, not a claim that they should be removed from the production application.

---

## Logger/Mutex Isolation Result

With both logging and benchmark mutex locking disabled, the same 5,306-event simulation replay was measured over 10 repetitions.

The result was approximately:

```text
Mean time:         533,559 ns
Mean throughput:   10.0361 M events/sec
Median time:       499,360 ns
Median throughput: 10.6259 M events/sec
Standard deviation:57,455 ns
CV:                10.77%
Events/replay:     5,306
```

Compared with the original baseline:

```text
                         Original       Logger/Mutex Disabled
Mean time                1.112 ms       0.534 ms
Mean throughput          4.79 M/s       10.04 M/s
```

The isolated configuration achieved approximately **2.1× the throughput** of the original configuration.

This demonstrates that logging and/or mutex synchronization contribute substantial overhead to this single-threaded benchmark workload.

Because both were disabled simultaneously, this experiment does **not** independently quantify the contribution of logging versus mutexes. A separate controlled experiment would be required to isolate each component individually.

### Important Benchmarking Note

The benchmark environment reported:

```text
CPU scaling is enabled
```

Therefore, individual runs can vary due to CPU frequency changes and other system activity.

For this reason, repeated measurements, mean/median values, and the coefficient of variation are recorded instead of relying on a single benchmark run.

---

## Current Limitations

The project is an experimental implementation intended for learning, research, and further development.

Current limitations include:

* Order insertion and cancellation rely on linked-list traversal.
* There is currently no hash map for direct order-ID lookup.
* The order book does not yet use a dedicated price-level data structure.
* The simulation uses a fixed delay between events.
* No real exchange connectivity is currently implemented.
* No live market-data feed is currently integrated.
* The logging and external publishing components are still under development.

---

## Future Development

The project provides a foundation for experimenting with more advanced market-microstructure concepts and trading-system components.

Potential extensions include:

* More efficient order lookup and cancellation
* Dedicated price-level structures
* Market-order support
* Additional order types
* Real-time market-data ingestion
* Deterministic event replay
* Latency and throughput benchmarking
* Order-flow analytics
* Market-depth analytics
* Order Flow Imbalance (OFI)
* Advanced execution and trading models

---

## Why Build a Limit Order Book?

A Limit Order Book is one of the fundamental components of an electronic financial market.

Implementing one from scratch provides a practical understanding of:

* Price-time priority
* Bid-ask spreads
* Market depth
* Liquidity
* Partial execution
* Order cancellation
* Trade formation
* Matching-engine architecture
* Market microstructure

This project serves as a foundation for experimenting with **quantitative trading, market microstructure, and high-performance trading-system design**.

---

## License

This project is distributed under the MIT License.
