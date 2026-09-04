#include "tui.h"
#include "dashboard.h"
#include "tradelog.h"

#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

#include <atomic>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <thread>
#include <algorithm>

using namespace ftxui;

static std::atomic<bool> running{true};
static ScreenInteractive* screen = nullptr;

void refreshBook()
{
    if (screen) {
        screen->PostEvent(Event::Custom);
    }
}

void stopTUI()
{
    running = false;

    if (screen) {
        screen->Exit();
    }
}

static std::string formatPrice(double price)
{
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(2) << price;
    return ss.str();
}

static std::string formatQuantity(int quantity)
{
    return std::to_string(quantity);
}

static std::string formatSide(char side)
{
    return side == 'B' ? "BUY" : "SELL";
}

void startTUI()
{
    auto localScreen = ScreenInteractive::Fullscreen();
    screen = &localScreen;

    auto renderer = Renderer([&] {

        auto bids = getBidLevels();
        auto asks = getAskLevels();

        double bestBid = getBestBid();
        double bestAsk = getBestAsk();
        double spread = getSpread();
        double midPrice = getMidPrice();

        Elements tradeRows;

        tradeRows.push_back(
            hbox({
                text("PRICE") | size(WIDTH, EQUAL, 12),
                text("SIDE")  | size(WIDTH, EQUAL, 10),
                text("QTY")
            }) | bold
        );

        {
            std::lock_guard<std::mutex> lock(tradeMutex);

            int start = std::max(0, static_cast<int>(tradeLog.size()) - 8);

            for (int i = static_cast<int>(tradeLog.size()) - 1;
                i >= start;
                --i) {

                const auto& trade = tradeLog[i];

                tradeRows.push_back(
                    hbox({
                        text(formatPrice(trade.price))
                            | size(WIDTH, EQUAL, 12),

                        text(formatSide(trade.side))
                            | size(WIDTH, EQUAL, 10),

                        text(formatQuantity(trade.quantity))
                    })
                );
            }
        }

        // --------------------------------------------------
        // ASK TABLE
        // --------------------------------------------------

        Elements askRows;

        askRows.push_back(
            hbox({
                text("QTY")   | size(WIDTH, EQUAL, 10),
                text("PRICE") | size(WIDTH, EQUAL, 10)
            }) | bold
        );

        for (const auto& level : asks) {
            askRows.push_back(
                hbox({
                    text(formatQuantity(level.quantity))
                        | size(WIDTH, EQUAL, 10)
                        | align_right,
                    text(formatPrice(level.price))
                })
            );
        }

        // --------------------------------------------------
        // BID TABLE
        // --------------------------------------------------

        Elements bidRows;

        bidRows.push_back(
            hbox({
                text("PRICE") | size(WIDTH, EQUAL, 10),
                text("QTY")   | size(WIDTH, EQUAL, 10)
            }) | bold
        );

        for (const auto& level : bids) {
            bidRows.push_back(
                hbox({
                    text(formatPrice(level.price))
                        | size(WIDTH, EQUAL, 12)
                        | align_right,
                    text(formatQuantity(level.quantity))
                })
            );
        }

        // --------------------------------------------------
        // MARKET INFORMATION
        // --------------------------------------------------

        auto marketInfo = vbox({

            hbox({
                text("Best Bid : ") | bold,
                text(formatPrice(bestBid))
            }),

            hbox({
                text("Best Ask : ") | bold,
                text(formatPrice(bestAsk))
            }),

            hbox({
                text("Spread   : ") | bold,
                text(formatPrice(spread))
            }),

            hbox({
                text("Mid Price: ") | bold,
                text(formatPrice(midPrice))
            })
        });

        // --------------------------------------------------
        // FINAL UI
        // --------------------------------------------------

        return vbox({

            text("LIMIT ORDER BOOK")
                | bold
                | center
                | border,

            hbox({

                window(
                    text("ASKS"),
                    vbox(askRows)
                )
                | flex,

                window(
                    text("BIDS"),
                    vbox(bidRows)
                )
                | flex

            }) | flex,

            separator(),

            marketInfo | center | border,

            window(
                text("RECENT TRADES"),
                vbox(tradeRows)
            ) | border,

            text("Press Q to quit") | center
            
        });
    });

    auto component = CatchEvent(
        renderer,
        [&](Event event) {

            if (event == Event::Character("q") ||
                event == Event::Character("Q")) {

                stopTUI();
                return true;
            }

            return false;
        }
    );

    localScreen.Loop(component);

    screen = nullptr;
}