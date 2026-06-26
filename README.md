# Limit Order Book (LOB)

A focused, high-performance C++ implementation of a financial Limit Order Book designed to store and manage limit orders based strictly on **price-time priority**.

> ⚠️ **Work in Progress** — Core structures, memory management, and algorithmic efficiencies are being continuously refined. Feedback, code reviews, and architectural suggestions are very welcome! Open an issue or submit a PR if you have ideas on container choices, queue mechanics, or performance improvements.

---

## Overview

This repository focuses on the low-level data structure mechanics required to maintain an electronic trading order book. Its objective is to accept buy (`Bids`) and sell (`Asks`) limit orders, organize them efficiently by price tier, and maintain strict chronological sequence (time priority) within each level.

### Core Mechanics

- **Limit Order Storage** — Segregates and stores active limit orders on the correct book side using cache-friendly layouts.
- **Price-Time Priority (FIFO)** — Orders are queued sequentially by arrival at each price point. First-in orders get execution and cancellation priority.

---

## Architecture

The book is structured in hierarchical layers to achieve predictable algorithmic complexity without traditional lookup structures.

### 1. Order Entity
Represents an individual resting limit order.

| Field | Description |
|-------|-------------|
| `id` | Unique identifier for tracking |
| `price` | Limit price defining execution tier |
| `quantity` | Outstanding volume/size |
| `side` | Buy (Bid) or Sell (Ask) |

### 2. Price Levels
A collection of orders at the same price point, organized chronologically to satisfy time priority. Implemented using custom intrusive doubly-linked lists for fast deletion when volume is cancelled.

### 3. Book Representation
The top-level interface managing the full structure. Segregates orders into side-specific price structures, ensuring the best bid and best ask are always correctly ordered relative to each other.

---

## API

| Method | Description |
|--------|-------------|
| `Add` | Inserts a resting limit order at the target price tier, allocating a new level if needed |
| `Cancel` | Removes an active order from its chronological slot |
| `Update` | Modifies resting volume in-place without losing queue position (for reductions) |

---

## Getting Started

### Prerequisites

- **GCC** (v8.1+) or **Clang** (v7+) with C++17 support
- **CMake** (v3.15+) *(optional)*

### Build

```bash
git clone https://github.com/atharvsalvi/limit-order-book.git
cd limit-order-book
```

**Using CMake:**
```bash
mkdir build && cd build
cmake ..
cmake --build .
```

**Using g++ directly:**
```bash
g++ -std=c++17 main.cpp src/*.cpp -I include -o limit_order_book
```

### Run

```bash
./limit_order_book
```

---

## Contributing

External ideas and discussion are highly valued. Topics of interest include memory alignment, container alternatives to hash maps, and low-latency layout paradigms.

- **Open an issue** labeled `suggestion` to discuss data layouts or optimization strategies
- **Fork and submit a PR** for concrete code improvements or benchmarks

---

## License

Distributed under the [MIT License](LICENSE).