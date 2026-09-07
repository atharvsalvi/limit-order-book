#include "simulation.h"

void Simulation::process(const MarketEvent& event) {

    if (event.type == EventType::ADD) {

        if (event.side == Side::BUY) {

            addBuyer(
                new OrderCard,
                event.id,
                event.price,
                event.quantity,
                event.timestamp
            );

        }
        else if (event.side == Side::SELL) {

            addSeller(
                new OrderCard,
                event.id,
                event.price,
                event.quantity,
                event.timestamp
            );
        }
    }

    else if (event.type == EventType::CANCEL) {

        cancel_order(event.id);
    }
}