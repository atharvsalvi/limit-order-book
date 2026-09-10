#include <gtest/gtest.h>

#include "engine/orderbook.h"
#include "tradelog/tradelog.h"

class OrderBookTest : public ::testing::Test {
protected:

    void SetUp() override
    {
        // Every test starts with an empty order book
        buyHead = nullptr;
        sellHead = nullptr;

        // Every test starts with an empty trade log
        tradeLog.clear();
    }

    void TearDown() override
    {
        // Clean up remaining buy orders
        while (buyHead != nullptr) {
            OrderCard* temp = buyHead;
            buyHead = buyHead->next;
            delete temp;
        }

        // Clean up remaining sell orders
        while (sellHead != nullptr) {
            OrderCard* temp = sellHead;
            sellHead = sellHead->next;
            delete temp;
        }

        // Clean up trades
        tradeLog.clear();
    }
};

// Checks buy orders addition
TEST_F(OrderBookTest, AddBuyOrder)
{
    auto now = std::chrono::system_clock::now();

    OrderCard* buyer = new OrderCard;

    addBuyer(buyer, 1, 100.0, 10, now);

    ASSERT_NE(buyHead, nullptr);

    EXPECT_EQ(buyHead->orderID, 1);
    EXPECT_DOUBLE_EQ(buyHead->price, 100.0);
    EXPECT_EQ(buyHead->quantity, 10);
}

// Checks sell orders addition
TEST_F(OrderBookTest, AddSellOrder)
{
    auto now = std::chrono::system_clock::now();

    OrderCard* seller = new OrderCard;

    addSeller(seller, 2, 101.0, 20, now);

    ASSERT_NE(sellHead, nullptr);

    EXPECT_EQ(sellHead->orderID, 2);
    EXPECT_DOUBLE_EQ(sellHead->price, 101.0);
    EXPECT_EQ(sellHead->quantity, 20);
}

// Price priority matching test
TEST_F(OrderBookTest, BuyPricePriority)
{
    auto now = std::chrono::system_clock::now();

    OrderCard* buyer1 = new OrderCard;
    OrderCard* buyer2 = new OrderCard;
    OrderCard* buyer3 = new OrderCard;

    addBuyer(buyer1, 1, 103.0, 10, now);
    addBuyer(buyer2, 2, 105.0, 10, now);
    addBuyer(buyer3, 3, 100.0, 10, now);

    ASSERT_NE(buyHead, nullptr);
    ASSERT_NE(buyHead->next, nullptr);
    ASSERT_NE(buyHead->next->next, nullptr);

    EXPECT_DOUBLE_EQ(buyHead->price, 105.0);
    EXPECT_DOUBLE_EQ(buyHead->next->price, 103.0);
    EXPECT_DOUBLE_EQ(buyHead->next->next->price, 100.0);
}
TEST_F(OrderBookTest, SellPricePriority)
{
    auto now = std::chrono::system_clock::now();

    OrderCard* seller1 = new OrderCard;
    OrderCard* seller2 = new OrderCard;
    OrderCard* seller3 = new OrderCard;

    addSeller(seller1, 1, 103.0, 10, now);
    addSeller(seller2, 2, 105.0, 10, now);
    addSeller(seller3, 3, 100.0, 10, now);

    ASSERT_NE(sellHead, nullptr);
    ASSERT_NE(sellHead->next, nullptr);
    ASSERT_NE(sellHead->next->next, nullptr);

    EXPECT_DOUBLE_EQ(sellHead->price, 100.0);
    EXPECT_DOUBLE_EQ(sellHead->next->price, 103.0);
    EXPECT_DOUBLE_EQ(sellHead->next->next->price, 105.0);
}

// Time priority matching test
TEST_F(OrderBookTest, BuyTimePriority)
{
    auto now = std::chrono::system_clock::now();

    OrderCard* buyer1 = new OrderCard;
    OrderCard* buyer2 = new OrderCard;
    OrderCard* buyer3 = new OrderCard;

    addBuyer(buyer1, 1, 100.0, 10, now);
    addBuyer(buyer2, 2, 105.0, 10, now);
    addBuyer(buyer3, 3, 100.0, 10, now);

    ASSERT_NE(buyHead, nullptr);
    ASSERT_NE(buyHead->next, nullptr);
    ASSERT_NE(buyHead->next->next, nullptr);

    EXPECT_DOUBLE_EQ(buyHead->price, 105.0);
    EXPECT_EQ(buyHead->orderID, 2);

    EXPECT_DOUBLE_EQ(buyHead->next->price, 100.0);
    EXPECT_EQ(buyHead->next->orderID, 1);

    EXPECT_DOUBLE_EQ(buyHead->next->next->price, 100.0);
    EXPECT_EQ(buyHead->next->next->orderID, 3);
}
TEST_F(OrderBookTest, SellTimePriority)
{
    auto now = std::chrono::system_clock::now();

    OrderCard* seller1 = new OrderCard;
    OrderCard* seller2 = new OrderCard;
    OrderCard* seller3 = new OrderCard;

    addSeller(seller1, 1, 102.0, 10, now);
    addSeller(seller2, 2, 110.0, 10, now);
    addSeller(seller3, 3, 102.0, 10, now);

    ASSERT_NE(sellHead, nullptr);
    ASSERT_NE(sellHead->next, nullptr);
    ASSERT_NE(sellHead->next->next, nullptr);

    EXPECT_DOUBLE_EQ(sellHead->price, 102.0);
    EXPECT_EQ(sellHead->orderID, 1);

    EXPECT_DOUBLE_EQ(sellHead->next->price, 102.0);
    EXPECT_EQ(sellHead->next->orderID, 3);

    EXPECT_DOUBLE_EQ(sellHead->next->next->price, 110.0);
    EXPECT_EQ(sellHead->next->next->orderID, 2);
}

// Partial fills
TEST_F(OrderBookTest, PartialFillBuyOrder)
{
    auto buyTime = std::chrono::system_clock::now();
    auto sellTime = buyTime + std::chrono::milliseconds(1);

    OrderCard* buyer = new OrderCard;
    OrderCard* seller = new OrderCard;

    addBuyer(buyer, 1, 100.0, 10, buyTime);
    addSeller(seller, 2, 100.0, 4, sellTime);

    ASSERT_NE(buyHead, nullptr);
    EXPECT_EQ(buyHead->orderID, 1);
    EXPECT_EQ(buyHead->quantity, 6);

    EXPECT_EQ(sellHead, nullptr);

    ASSERT_EQ(tradeLog.size(), 1);
    EXPECT_EQ(tradeLog[0].quantity, 4);
    EXPECT_DOUBLE_EQ(tradeLog[0].price, 100.0);
    EXPECT_EQ(tradeLog[0].side, 'S');
}
TEST_F(OrderBookTest, PartialFillSellOrder)
{
    auto sellTime = std::chrono::system_clock::now();
    auto buyTime = sellTime + std::chrono::milliseconds(1);

    OrderCard* seller = new OrderCard;
    OrderCard* buyer = new OrderCard;

    addSeller(seller, 2, 100.0, 10, sellTime);
    addBuyer(buyer, 1, 100.0, 4, buyTime);

    EXPECT_EQ(buyHead, nullptr);

    ASSERT_NE(sellHead, nullptr);
    EXPECT_EQ(sellHead->orderID, 2);
    EXPECT_EQ(sellHead->quantity, 6);

    ASSERT_EQ(tradeLog.size(), 1);
    EXPECT_EQ(tradeLog[0].quantity, 4);
    EXPECT_DOUBLE_EQ(tradeLog[0].price, 100.0);
    EXPECT_EQ(tradeLog[0].side, 'B');
}

// Full Fills
TEST_F(OrderBookTest, FullFillBuyerAggressor)
{
    auto sellTime = std::chrono::system_clock::now();
    auto buyTime = sellTime + std::chrono::milliseconds(1);

    OrderCard* seller = new OrderCard;
    OrderCard* buyer = new OrderCard;

    addSeller(seller, 1, 100.0, 10, sellTime);
    addBuyer(buyer, 2, 100.0, 10, buyTime);

    // Both orders should be completely filled
    EXPECT_EQ(buyHead, nullptr);
    EXPECT_EQ(sellHead, nullptr);

    // Exactly one trade should have occurred
    ASSERT_EQ(tradeLog.size(), 1);

    EXPECT_DOUBLE_EQ(tradeLog[0].price, 100.0);
    EXPECT_EQ(tradeLog[0].quantity, 10);
    EXPECT_EQ(tradeLog[0].side, 'B');
}
TEST_F(OrderBookTest, FullFillSellerAggressor)
{
    auto buyTime = std::chrono::system_clock::now();
    auto sellTime = buyTime + std::chrono::milliseconds(1);

    OrderCard* buyer = new OrderCard;
    OrderCard* seller = new OrderCard;

    addBuyer(buyer, 1, 100.0, 10, buyTime);
    addSeller(seller, 2, 100.0, 10, sellTime);

    // Both orders should be completely filled
    EXPECT_EQ(buyHead, nullptr);
    EXPECT_EQ(sellHead, nullptr);

    // Exactly one trade should have occurred
    ASSERT_EQ(tradeLog.size(), 1);

    EXPECT_DOUBLE_EQ(tradeLog[0].price, 100.0);
    EXPECT_EQ(tradeLog[0].quantity, 10);
    EXPECT_EQ(tradeLog[0].side, 'S');
}

// Multiple Fills
TEST_F(OrderBookTest, MultipleFillsBuyerAggressor)
{
    auto time1 = std::chrono::system_clock::now();
    auto time2 = time1 + std::chrono::milliseconds(1);
    auto time3 = time2 + std::chrono::milliseconds(1);

    OrderCard* seller1 = new OrderCard;
    OrderCard* seller2 = new OrderCard;
    OrderCard* buyer = new OrderCard;

    // Resting sell orders
    addSeller(seller1, 1, 100.0, 5, time1);
    addSeller(seller2, 2, 101.0, 7, time2);

    // Incoming BUY aggressor
    addBuyer(buyer, 3, 101.0, 10, time3);

    // BUY should be completely filled
    EXPECT_EQ(buyHead, nullptr);

    // Second SELL order should have 2 remaining
    ASSERT_NE(sellHead, nullptr);

    EXPECT_EQ(sellHead->orderID, 2);
    EXPECT_DOUBLE_EQ(sellHead->price, 101.0);
    EXPECT_EQ(sellHead->quantity, 2);

    // Exactly two executions
    ASSERT_EQ(tradeLog.size(), 2);

    // First execution: SELL #1
    EXPECT_DOUBLE_EQ(tradeLog[0].price, 100.0);
    EXPECT_EQ(tradeLog[0].quantity, 5);
    EXPECT_EQ(tradeLog[0].side, 'B');

    // Second execution: SELL #2
    EXPECT_DOUBLE_EQ(tradeLog[1].price, 101.0);
    EXPECT_EQ(tradeLog[1].quantity, 5);
    EXPECT_EQ(tradeLog[1].side, 'B');
}
TEST_F(OrderBookTest, MultipleFillsSellerAggressor)
{
    auto time1 = std::chrono::system_clock::now();
    auto time2 = time1 + std::chrono::milliseconds(1);
    auto time3 = time2 + std::chrono::milliseconds(1);

    OrderCard* buyer1 = new OrderCard;
    OrderCard* buyer2 = new OrderCard;
    OrderCard* seller = new OrderCard;

    // Resting BUY orders
    addBuyer(buyer1, 1, 101.0, 5, time1);
    addBuyer(buyer2, 2, 100.0, 7, time2);

    // Incoming SELL aggressor
    addSeller(seller, 3, 100.0, 10, time3);

    // SELL should be completely filled
    EXPECT_EQ(sellHead, nullptr);

    // Second BUY should have 2 remaining
    ASSERT_NE(buyHead, nullptr);

    EXPECT_EQ(buyHead->orderID, 2);
    EXPECT_DOUBLE_EQ(buyHead->price, 100.0);
    EXPECT_EQ(buyHead->quantity, 2);

    // Exactly two executions
    ASSERT_EQ(tradeLog.size(), 2);

    // First execution: BUY #1
    EXPECT_DOUBLE_EQ(tradeLog[0].price, 101.0);
    EXPECT_EQ(tradeLog[0].quantity, 5);
    EXPECT_EQ(tradeLog[0].side, 'S');

    // Second execution: BUY #2
    EXPECT_DOUBLE_EQ(tradeLog[1].price, 100.0);
    EXPECT_EQ(tradeLog[1].quantity, 5);
    EXPECT_EQ(tradeLog[1].side, 'S');
}

// Order Cancellation
TEST_F(OrderBookTest, CancelOnlyBuyOrder)
{
    auto now = std::chrono::system_clock::now();

    OrderCard* buyer = new OrderCard;

    addBuyer(buyer, 1, 100.0, 10, now);

    ASSERT_NE(buyHead, nullptr);

    cancel_order(1);

    EXPECT_EQ(buyHead, nullptr);
}
TEST_F(OrderBookTest, CancelBuyHead)
{
    auto now = std::chrono::system_clock::now();

    OrderCard* buyer1 = new OrderCard;
    OrderCard* buyer2 = new OrderCard;
    OrderCard* buyer3 = new OrderCard;

    addBuyer(buyer1, 1, 105.0, 10, now);
    addBuyer(buyer2, 2, 100.0, 10, now);
    addBuyer(buyer3, 3, 95.0, 10, now);

    cancel_order(1);

    ASSERT_NE(buyHead, nullptr);

    EXPECT_EQ(buyHead->orderID, 2);
    EXPECT_EQ(buyHead->price, 100.0);

    EXPECT_EQ(buyHead->prev, nullptr);

    ASSERT_NE(buyHead->next, nullptr);

    EXPECT_EQ(buyHead->next->orderID, 3);
    EXPECT_EQ(buyHead->next->prev, buyHead);
}
TEST_F(OrderBookTest, CancelBuyMiddle)
{
    auto now = std::chrono::system_clock::now();

    OrderCard* buyer1 = new OrderCard;
    OrderCard* buyer2 = new OrderCard;
    OrderCard* buyer3 = new OrderCard;

    addBuyer(buyer1, 1, 105.0, 10, now);
    addBuyer(buyer2, 2, 100.0, 10, now);
    addBuyer(buyer3, 3, 95.0, 10, now);

    cancel_order(2);

    ASSERT_NE(buyHead, nullptr);

    EXPECT_EQ(buyHead->orderID, 1);

    ASSERT_NE(buyHead->next, nullptr);

    EXPECT_EQ(buyHead->next->orderID, 3);

    EXPECT_EQ(buyHead->next->prev, buyHead);
    EXPECT_EQ(buyHead->next->next, nullptr);
}
TEST_F(OrderBookTest, CancelBuyTail)
{
    auto now = std::chrono::system_clock::now();

    OrderCard* buyer1 = new OrderCard;
    OrderCard* buyer2 = new OrderCard;
    OrderCard* buyer3 = new OrderCard;

    addBuyer(buyer1, 1, 105.0, 10, now);
    addBuyer(buyer2, 2, 100.0, 10, now);
    addBuyer(buyer3, 3, 95.0, 10, now);

    cancel_order(3);

    ASSERT_NE(buyHead, nullptr);

    EXPECT_EQ(buyHead->orderID, 1);

    ASSERT_NE(buyHead->next, nullptr);

    EXPECT_EQ(buyHead->next->orderID, 2);
    EXPECT_EQ(buyHead->next->next, nullptr);

    EXPECT_EQ(buyHead->next->prev, buyHead);
}
TEST_F(OrderBookTest, CancelNonexistentBuyOrder)
{
    auto now = std::chrono::system_clock::now();

    OrderCard* buyer1 = new OrderCard;
    OrderCard* buyer2 = new OrderCard;

    addBuyer(buyer1, 1, 105.0, 10, now);
    addBuyer(buyer2, 2, 100.0, 10, now);

    cancel_order(999);

    ASSERT_NE(buyHead, nullptr);

    EXPECT_EQ(buyHead->orderID, 1);

    ASSERT_NE(buyHead->next, nullptr);

    EXPECT_EQ(buyHead->next->orderID, 2);
}

TEST_F(OrderBookTest, CancelOnlySellOrder)
{
    auto now = std::chrono::system_clock::now();

    OrderCard* seller = new OrderCard;

    addSeller(seller, 1, 100.0, 10, now);

    ASSERT_NE(sellHead, nullptr);

    cancel_order(1);

    EXPECT_EQ(sellHead, nullptr);
}
TEST_F(OrderBookTest, CancelSellHead)
{
    auto now = std::chrono::system_clock::now();

    OrderCard* seller1 = new OrderCard;
    OrderCard* seller2 = new OrderCard;
    OrderCard* seller3 = new OrderCard;

    addSeller(seller1, 1, 105.0, 10, now);
    addSeller(seller2, 2, 100.0, 10, now);
    addSeller(seller3, 3, 95.0, 10, now);

    cancel_order(3);

    ASSERT_NE(sellHead, nullptr);

    EXPECT_EQ(sellHead->orderID, 2);
    EXPECT_EQ(sellHead->price, 100.0);

    EXPECT_EQ(sellHead->prev, nullptr);

    ASSERT_NE(sellHead->next, nullptr);

    EXPECT_EQ(sellHead->next->orderID, 1);
    EXPECT_EQ(sellHead->next->prev, sellHead);
}
TEST_F(OrderBookTest, CancelSellMiddle)
{
    auto now = std::chrono::system_clock::now();

    OrderCard* seller1 = new OrderCard;
    OrderCard* seller2 = new OrderCard;
    OrderCard* seller3 = new OrderCard;

    addSeller(seller1, 1, 105.0, 10, now);
    addSeller(seller2, 2, 100.0, 10, now);
    addSeller(seller3, 3, 95.0, 10, now);

    cancel_order(2);

    ASSERT_NE(sellHead, nullptr);

    EXPECT_EQ(sellHead->orderID, 3);

    ASSERT_NE(sellHead->next, nullptr);

    EXPECT_EQ(sellHead->next->orderID, 1);

    EXPECT_EQ(sellHead->next->prev, sellHead);
    EXPECT_EQ(sellHead->next->next, nullptr);
}
TEST_F(OrderBookTest, CancelSellTail)
{
    auto now = std::chrono::system_clock::now();

    OrderCard* seller1 = new OrderCard;
    OrderCard* seller2 = new OrderCard;
    OrderCard* seller3 = new OrderCard;

    addSeller(seller1, 1, 105.0, 10, now);
    addSeller(seller2, 2, 100.0, 10, now);
    addSeller(seller3, 3, 95.0, 10, now);

    cancel_order(1);

    ASSERT_NE(sellHead, nullptr);

    EXPECT_EQ(sellHead->orderID, 3);

    ASSERT_NE(sellHead->next, nullptr);

    EXPECT_EQ(sellHead->next->orderID, 2);
    EXPECT_EQ(sellHead->next->next, nullptr);

    EXPECT_EQ(sellHead->next->prev, sellHead);
}
TEST_F(OrderBookTest, CancelNonexistentSellOrder)
{
    auto now = std::chrono::system_clock::now();

    OrderCard* seller1 = new OrderCard;
    OrderCard* seller2 = new OrderCard;

    addSeller(seller1, 1, 105.0, 10, now);
    addSeller(seller2, 2, 100.0, 10, now);

    cancel_order(999);

    ASSERT_NE(sellHead, nullptr);

    EXPECT_EQ(sellHead->orderID, 2);

    ASSERT_NE(sellHead->next, nullptr);

    EXPECT_EQ(sellHead->next->orderID, 1);
}

// No Match Scenario
TEST_F(OrderBookTest, NoMatchWhenPricesDoNotCross)
{
    auto buyTime = std::chrono::system_clock::now();
    auto sellTime = buyTime + std::chrono::milliseconds(1);

    OrderCard* buyer = new OrderCard;
    OrderCard* seller = new OrderCard;

    addBuyer(buyer, 1, 100.0, 10, buyTime);
    addSeller(seller, 2, 101.0, 10, sellTime);

    ASSERT_NE(buyHead, nullptr);
    ASSERT_NE(sellHead, nullptr);

    EXPECT_EQ(buyHead->orderID, 1);
    EXPECT_EQ(buyHead->quantity, 10);

    EXPECT_EQ(sellHead->orderID, 2);
    EXPECT_EQ(sellHead->quantity, 10);

    EXPECT_TRUE(tradeLog.empty());
}

// Equal Price Scenario
TEST_F(OrderBookTest, MatchWhenPricesAreEqual)
{
    auto buyTime = std::chrono::system_clock::now();
    auto sellTime = buyTime + std::chrono::milliseconds(1);

    OrderCard* buyer = new OrderCard;
    OrderCard* seller = new OrderCard;

    addBuyer(buyer, 1, 100.0, 10, buyTime);
    addSeller(seller, 2, 100.0, 10, sellTime);

    EXPECT_EQ(buyHead, nullptr);
    EXPECT_EQ(sellHead, nullptr);

    ASSERT_EQ(tradeLog.size(), 1);

    EXPECT_DOUBLE_EQ(tradeLog[0].price, 100.0);
    EXPECT_EQ(tradeLog[0].quantity, 10);
    EXPECT_EQ(tradeLog[0].side, 'S');
}

// Links Integrity Test
TEST_F(OrderBookTest, BuyLinkedListIntegrity)
{
    auto now = std::chrono::system_clock::now();

    OrderCard* buyer1 = new OrderCard;
    OrderCard* buyer2 = new OrderCard;
    OrderCard* buyer3 = new OrderCard;

    addBuyer(buyer1, 1, 105.0, 10, now);
    addBuyer(buyer2, 2, 100.0, 10, now);
    addBuyer(buyer3, 3, 95.0, 10, now);

    ASSERT_NE(buyHead, nullptr);
    ASSERT_NE(buyHead->next, nullptr);
    ASSERT_NE(buyHead->next->next, nullptr);

    OrderCard* first = buyHead;
    OrderCard* second = first->next;
    OrderCard* third = second->next;

    // Forward links
    EXPECT_EQ(first->next, second);
    EXPECT_EQ(second->next, third);
    EXPECT_EQ(third->next, nullptr);

    // Backward links
    EXPECT_EQ(first->prev, nullptr);
    EXPECT_EQ(second->prev, first);
    EXPECT_EQ(third->prev, second);
}
TEST_F(OrderBookTest, SellLinkedListIntegrity)
{
    auto now = std::chrono::system_clock::now();

    OrderCard* seller1 = new OrderCard;
    OrderCard* seller2 = new OrderCard;
    OrderCard* seller3 = new OrderCard;

    addSeller(seller1, 1, 100.0, 10, now);
    addSeller(seller2, 2, 103.0, 10, now);
    addSeller(seller3, 3, 105.0, 10, now);

    ASSERT_NE(sellHead, nullptr);
    ASSERT_NE(sellHead->next, nullptr);
    ASSERT_NE(sellHead->next->next, nullptr);

    OrderCard* first = sellHead;
    OrderCard* second = first->next;
    OrderCard* third = second->next;

    // Forward links
    EXPECT_EQ(first->next, second);
    EXPECT_EQ(second->next, third);
    EXPECT_EQ(third->next, nullptr);

    // Backward links
    EXPECT_EQ(first->prev, nullptr);
    EXPECT_EQ(second->prev, first);
    EXPECT_EQ(third->prev, second);
}

// After canceling an order, the links of the remaining orders should remain intact
TEST_F(OrderBookTest, BuyLinksRemainValidAfterCancellingHead)
{
    auto now = std::chrono::system_clock::now();

    OrderCard* buyer1 = new OrderCard;
    OrderCard* buyer2 = new OrderCard;
    OrderCard* buyer3 = new OrderCard;

    addBuyer(buyer1, 1, 105.0, 10, now);
    addBuyer(buyer2, 2, 100.0, 10, now);
    addBuyer(buyer3, 3, 95.0, 10, now);

    cancel_order(1);

    ASSERT_NE(buyHead, nullptr);
    ASSERT_NE(buyHead->next, nullptr);

    // New head should be #2
    EXPECT_EQ(buyHead->orderID, 2);

    // Head has no previous node
    EXPECT_EQ(buyHead->prev, nullptr);

    // #2 should point forward to #3
    EXPECT_EQ(buyHead->next->orderID, 3);

    // #3 should point backward to #2
    EXPECT_EQ(buyHead->next->prev, buyHead);

    // #3 is the tail
    EXPECT_EQ(buyHead->next->next, nullptr);
}
TEST_F(OrderBookTest, BuyLinksRemainValidAfterCancellingMiddle)
{
    auto now = std::chrono::system_clock::now();

    OrderCard* buyer1 = new OrderCard;
    OrderCard* buyer2 = new OrderCard;
    OrderCard* buyer3 = new OrderCard;

    addBuyer(buyer1, 1, 105.0, 10, now);
    addBuyer(buyer2, 2, 100.0, 10, now);
    addBuyer(buyer3, 3, 95.0, 10, now);

    cancel_order(2);

    ASSERT_NE(buyHead, nullptr);
    ASSERT_NE(buyHead->next, nullptr);

    OrderCard* first = buyHead;
    OrderCard* second = buyHead->next;

    EXPECT_EQ(first->orderID, 1);
    EXPECT_EQ(second->orderID, 3);

    // #1 → #3
    EXPECT_EQ(first->next, second);

    // #3 → #1
    EXPECT_EQ(second->prev, first);

    // Ends of list
    EXPECT_EQ(first->prev, nullptr);
    EXPECT_EQ(second->next, nullptr);
}
TEST_F(OrderBookTest, BuyLinksRemainValidAfterCancellingTail)
{
    auto now = std::chrono::system_clock::now();

    OrderCard* buyer1 = new OrderCard;
    OrderCard* buyer2 = new OrderCard;
    OrderCard* buyer3 = new OrderCard;

    addBuyer(buyer1, 1, 105.0, 10, now);
    addBuyer(buyer2, 2, 100.0, 10, now);
    addBuyer(buyer3, 3, 95.0, 10, now);

    cancel_order(3);

    ASSERT_NE(buyHead, nullptr);
    ASSERT_NE(buyHead->next, nullptr);

    OrderCard* first = buyHead;
    OrderCard* second = buyHead->next;

    EXPECT_EQ(first->orderID, 1);
    EXPECT_EQ(second->orderID, 2);

    // #1 → #2
    EXPECT_EQ(first->next, second);

    // #2 → #1
    EXPECT_EQ(second->prev, first);

    // Ends of list
    EXPECT_EQ(first->prev, nullptr);
    EXPECT_EQ(second->next, nullptr);
}
TEST_F(OrderBookTest, SellLinksRemainValidAfterCancellingHead)
{
    auto now = std::chrono::system_clock::now();

    OrderCard* seller1 = new OrderCard;
    OrderCard* seller2 = new OrderCard;
    OrderCard* seller3 = new OrderCard;

    addSeller(seller1, 1, 100.0, 10, now);
    addSeller(seller2, 2, 103.0, 10, now);
    addSeller(seller3, 3, 105.0, 10, now);

    cancel_order(1);

    ASSERT_NE(sellHead, nullptr);
    ASSERT_NE(sellHead->next, nullptr);

    EXPECT_EQ(sellHead->orderID, 2);
    EXPECT_EQ(sellHead->prev, nullptr);

    EXPECT_EQ(sellHead->next->orderID, 3);
    EXPECT_EQ(sellHead->next->prev, sellHead);
    EXPECT_EQ(sellHead->next->next, nullptr);
}
TEST_F(OrderBookTest, SellLinksRemainValidAfterCancellingMiddle)
{
    auto now = std::chrono::system_clock::now();

    OrderCard* seller1 = new OrderCard;
    OrderCard* seller2 = new OrderCard;
    OrderCard* seller3 = new OrderCard;

    addSeller(seller1, 1, 100.0, 10, now);
    addSeller(seller2, 2, 103.0, 10, now);
    addSeller(seller3, 3, 105.0, 10, now);

    cancel_order(2);

    ASSERT_NE(sellHead, nullptr);
    ASSERT_NE(sellHead->next, nullptr);

    OrderCard* first = sellHead;
    OrderCard* second = sellHead->next;

    EXPECT_EQ(first->orderID, 1);
    EXPECT_EQ(second->orderID, 3);

    // #1 → #3
    EXPECT_EQ(first->next, second);

    // #3 → #1
    EXPECT_EQ(second->prev, first);

    // Ends of list
    EXPECT_EQ(first->prev, nullptr);
    EXPECT_EQ(second->next, nullptr);
}
TEST_F(OrderBookTest, SellLinksRemainValidAfterCancellingTail)
{
    auto now = std::chrono::system_clock::now();

    OrderCard* seller1 = new OrderCard;
    OrderCard* seller2 = new OrderCard;
    OrderCard* seller3 = new OrderCard;

    addSeller(seller1, 1, 100.0, 10, now);
    addSeller(seller2, 2, 103.0, 10, now);
    addSeller(seller3, 3, 105.0, 10, now);

    cancel_order(3);

    ASSERT_NE(sellHead, nullptr);
    ASSERT_NE(sellHead->next, nullptr);

    OrderCard* first = sellHead;
    OrderCard* second = sellHead->next;

    EXPECT_EQ(first->orderID, 1);
    EXPECT_EQ(second->orderID, 2);

    // #1 → #2
    EXPECT_EQ(first->next, second);

    // #2 → #1
    EXPECT_EQ(second->prev, first);

    // Ends of list
    EXPECT_EQ(first->prev, nullptr);
    EXPECT_EQ(second->next, nullptr);
}

// Zero Quantity 
TEST_F(OrderBookTest, RejectZeroQuantity)
{
    auto now = std::chrono::system_clock::now();

    OrderCard* buyer = new OrderCard;

    addBuyer(buyer, 1, 100.0, 0, now);

    EXPECT_EQ(buyHead, nullptr);
    EXPECT_TRUE(tradeLog.empty());
}

// Negative Quantity
TEST_F(OrderBookTest, RejectNegativeQuantity)
{
    auto now = std::chrono::system_clock::now();

    OrderCard* buyer = new OrderCard;

    addBuyer(buyer, 1, 100.0, -5, now);

    EXPECT_EQ(buyHead, nullptr);
    EXPECT_TRUE(tradeLog.empty());
}

// Negative Price
TEST_F(OrderBookTest, RejectNegativePrice)
{
    auto now = std::chrono::system_clock::now();

    OrderCard* buyer = new OrderCard;

    addBuyer(buyer, 1, -100.0, 10, now);

    EXPECT_EQ(buyHead, nullptr);
    EXPECT_TRUE(tradeLog.empty());
}

// Cancel already filled order
TEST_F(OrderBookTest, CancelAlreadyFilledOrder)
{
    auto buyTime = std::chrono::system_clock::now();
    auto sellTime = buyTime + std::chrono::milliseconds(1);

    OrderCard* buyer = new OrderCard;
    OrderCard* seller = new OrderCard;

    addBuyer(buyer, 1, 100.0, 10, buyTime);
    addSeller(seller, 2, 100.0, 10, sellTime);

    // Verify the full fill happened
    EXPECT_EQ(buyHead, nullptr);
    EXPECT_EQ(sellHead, nullptr);
    ASSERT_EQ(tradeLog.size(), 1);

    // Try cancelling the already-filled BUY order
    cancel_order(1);

    // Book should still be empty
    EXPECT_EQ(buyHead, nullptr);
    EXPECT_EQ(sellHead, nullptr);

    // No additional trade should have been created
    EXPECT_EQ(tradeLog.size(), 1);
}

// Price execution test
TEST_F(OrderBookTest, BuyerAggressorExecutesAtRestingSellPrice)
{
    auto sellTime = std::chrono::system_clock::now();
    auto buyTime = sellTime + std::chrono::milliseconds(1);

    OrderCard* seller = new OrderCard;
    OrderCard* buyer = new OrderCard;

    addSeller(seller, 1, 100.0, 10, sellTime);
    addBuyer(buyer, 2, 105.0, 10, buyTime);

    EXPECT_EQ(buyHead, nullptr);
    EXPECT_EQ(sellHead, nullptr);

    ASSERT_EQ(tradeLog.size(), 1);

    EXPECT_DOUBLE_EQ(tradeLog[0].price, 100.0);
    EXPECT_EQ(tradeLog[0].quantity, 10);
    EXPECT_EQ(tradeLog[0].side, 'B');
}
TEST_F(OrderBookTest, SellerAggressorExecutesAtRestingBuyPrice)
{
    auto buyTime = std::chrono::system_clock::now();
    auto sellTime = buyTime + std::chrono::milliseconds(1);

    OrderCard* buyer = new OrderCard;
    OrderCard* seller = new OrderCard;

    addBuyer(buyer, 1, 100.0, 10, buyTime);
    addSeller(seller, 2, 95.0, 10, sellTime);

    EXPECT_EQ(buyHead, nullptr);
    EXPECT_EQ(sellHead, nullptr);

    ASSERT_EQ(tradeLog.size(), 1);

    EXPECT_DOUBLE_EQ(tradeLog[0].price, 100.0);
    EXPECT_EQ(tradeLog[0].quantity, 10);
    EXPECT_EQ(tradeLog[0].side, 'S');
}