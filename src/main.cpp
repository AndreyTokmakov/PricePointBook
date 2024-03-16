/**============================================================================
Name        : main.cpp
Created on  : 16.03.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : OrderBookNew.cpp
============================================================================**/

#include "PricePointBook.h"


int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    using namespace Types;
    const std::vector<std::string_view> args(argv + 1, argv + argc);

    PricePointBook::OrderBook book;

    // 0. Initialize snapshot
    //    Out of scope of the task: No live data connection or user input.
    Snapshot snapshot{};

    // 1. Load snapshots | Or Snapshots & Symbol
    book.loadSnapshot(snapshot, "BNBBTC");

    // 2. Initialize Event's list
    //    Out of scope of the task too
    std::vector<Event> events {};

    // 3. Handle events
    for (const Event& event: events)
        book.handleEvent(event);

    return EXIT_SUCCESS;
}
