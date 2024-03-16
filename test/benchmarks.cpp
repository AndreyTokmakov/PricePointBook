/**============================================================================
Name        : Benchmarks.cpp
Created on  : 16.04.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description :
============================================================================ **/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE PerformanceBenchmarkTest

#include <boost/test/unit_test.hpp>
#include <fstream>
#include <thread>

#include "Utilities.h"
#include "Types.h"
#include "PricePointBook.h"
#include "utils/TestUtils.h"


struct BaseFixture
{
    size_t ordersTotal = 0;
    uint64_t duration = 0;

    PricePointBook::OrderBook book{};

    static inline const std::string symbolDefault{"BNBBTC"};

    BaseFixture() = default;

    virtual ~BaseFixture()
    {
        std::cout << "\nProcessing the " << ordersTotal << " orders took " << duration << " microseconds\n";

        std::cout << "\tRate: " << static_cast<double>(ordersTotal) /static_cast<double>(duration)
            << " order/microsecond" << std::endl;
        std::cout << "\tRate: " << ordersTotal * 1'000'000 / duration << " orders/second" << std::endl;
    }
};


BOOST_AUTO_TEST_SUITE(Benchmarking)

    BOOST_FIXTURE_TEST_CASE(HandleEvents_BasicTest_SingleSymbol, BaseFixture)
    {
        Types::Snapshot snapshot;
        Utilities::parseSnapshot(R"(../../test/data/snapshot_BNBBTC_4000.json)", snapshot);

        /** Test data preparation and initialization **/
        const std::vector<Types::Event> testEvents { TestUtils::generateTestEvents(snapshot, symbolDefault) };
        ordersTotal = testEvents.size() * (testEvents.front().buyOrders.size() * 2);
        book.loadSnapshot(snapshot, symbolDefault);

        /** Running the test: **/
        const std::chrono::time_point start = std::chrono::high_resolution_clock::now();

        for (const Types::Event &event: testEvents)
            book.handleEvent(event);

        const std::chrono::time_point end = std::chrono::high_resolution_clock::now();

        /** Evaluating the result: **/
        duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    }

    BOOST_FIXTURE_TEST_CASE(HandleEvents_BasicTest_MultipleSymbols, BaseFixture)
    {
        Types::Snapshot snapshot;
        Utilities::parseSnapshot(R"(../../test/data/snapshot_BNBBTC_4000.json)", snapshot);

        /** Test data preparation and initialization **/
        constexpr size_t symbolsCount { 100 };
        const std::vector<std::string> symbols = TestUtils::getUniqueSymbols(symbolsCount);

        std::vector<std::vector<Types::Event>> testEventsForSymbols;
        testEventsForSymbols.reserve(symbolsCount);
        for (const std::string& symbol: symbols) {
            testEventsForSymbols.push_back(TestUtils::generateTestEvents(snapshot, symbol));
        }
        ordersTotal = symbolsCount * testEventsForSymbols.front().size() *
                (testEventsForSymbols.front().front().buyOrders.size() * 2);

        for (const std::string& symbol: symbols) {
            book.loadSnapshot(snapshot, symbol);
        }

        /** Running the test: **/
        const std::chrono::time_point start = std::chrono::high_resolution_clock::now();

        for (const std::vector<Types::Event>& testEvents: testEventsForSymbols) {
            for (const Types::Event &event: testEvents) {
                book.handleEvent(event);
            }
        }

        const std::chrono::time_point end = std::chrono::high_resolution_clock::now();

        /** Evaluating the results: **/
        duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    };

BOOST_AUTO_TEST_SUITE_END()
