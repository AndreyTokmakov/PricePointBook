/**============================================================================
Name        : memory_usage.cpp
Created on  : 16.04.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description :
============================================================================ **/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE MemoryConsumptionTest

#include <boost/test/unit_test.hpp>
#include <fstream>
#include <string_view>
#include <thread>
#include <atomic>

#include "Utilities.h"
#include "Types.h"
#include "PricePointBook.h"
#include "utils/TestUtils.h"

#include <nlohmann/json.hpp>

using namespace boost::unit_test::framework;

/** Simplest possible RAII based memory monitor **/
struct MemoryMonitor
{
    std::vector<std::string_view> arguments;

    double memInitial {0.0};
    double memUsedMax {0.0};

    std::atomic<bool> run {true};
    std::jthread memoryMonitor;

    int32_t symbolsCount { 100 };
    int32_t priceHitCount { 10 };
    int32_t priceMissCount { 4 };

    MemoryMonitor():
        arguments {master_test_suite().argv + 1, master_test_suite().argv + master_test_suite().argc },
        memInitial { TestUtils::getMemoryUsage() },
        memoryMonitor {std::jthread(&MemoryMonitor::monitor, this)}
    {
        if (arguments.size() > 0)
        {
            if (symbolsCount = atoi(arguments.front().data()); 0 >= symbolsCount) {
                symbolsCount = 100;
                std::cerr << "Incorrect number of symbols is given " << arguments.front()
                          << ". Using default value "  <<symbolsCount << std::endl;
                return;
            }
        }
        if (arguments.size() > 1)
        {
            if (priceHitCount = atoi(arguments[1].data()); 0 >= priceHitCount) {
                priceHitCount = 10;
                std::cerr << "Incorrect number of priceHitCount is given " << arguments[1]
                          << ". Using default value " << priceHitCount << std::endl;
                return;
            }
        }
        if (arguments.size() > 2)
        {
            if (priceMissCount = atoi(arguments[2].data()); 0 >= priceMissCount) {
                priceMissCount = 4;
                std::cerr << "Incorrect number of priceMissCount is given " << arguments[2]
                          << ". Using default value " << priceMissCount << std::endl;
                return;
            }
        }

        std::cout << "Test params: \n";
        std::cout << "\tDistinct symbols count : " << symbolsCount << "\n";
        std::cout << "\tPrice hit event count  : " << priceHitCount << "\n";
        std::cout << "\tPrice miss event count : " << priceMissCount << "\n";
    }

    ~MemoryMonitor()
    {
        run.store(false);
        memoryMonitor.join();

        /** Evaluating the results: **/
        std::cout << "Process used up to " << memUsedMax - memInitial << " Mbs of memory" << std::endl;
    }

    void monitor()
    {
        while (run) {
            memUsedMax = std::max(memUsedMax, TestUtils::getMemoryUsage());
            std::this_thread::sleep_for(std::chrono::microseconds (10));
        }
    }
};

BOOST_AUTO_TEST_SUITE(MemoryBenchmarking)

    BOOST_FIXTURE_TEST_CASE(LoadSnapshotsAndProcessEvents, MemoryMonitor)
    {
        PricePointBook::OrderBook book {};


        Types::Snapshot snapshot;
        Utilities::parseSnapshot(R"(../../test/data/snapshot_BNBBTC_4000.json)", snapshot);

        /** Test data preparation and initialization **/
        const std::vector<std::string> symbols = TestUtils::getUniqueSymbols(symbolsCount);

        std::vector<std::vector<Types::Event>> testEventsForSymbols;
        testEventsForSymbols.reserve(symbolsCount);
        for (const std::string& symbol: symbols) {
            testEventsForSymbols.push_back(TestUtils::generateTestEvents(snapshot, symbol, priceHitCount, priceMissCount));
        }

        /** Running the test: **/
        memInitial = TestUtils::getMemoryUsage();

        for (const std::string& symbol: symbols) {
            book.loadSnapshot(snapshot, symbol);
        }

        for (const std::vector<Types::Event>& testEvents: testEventsForSymbols) {
            for (const Types::Event &event: testEvents) {
                book.handleEvent(event);
            }
        }
    };

BOOST_AUTO_TEST_SUITE_END()
