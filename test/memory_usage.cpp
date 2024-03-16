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

/** Simplest possible RAII based memory monitor **/
struct MemoryMonitor
{
    std::atomic<bool> run {true};
    double memInitial {0.0};
    double memUsedMax {0.0};
    std::jthread memoryMonitor;

    MemoryMonitor(): memInitial { TestUtils::getMemoryUsage() },
                   memoryMonitor {std::jthread(&MemoryMonitor::monitor, this)} {
    }

    ~MemoryMonitor() {
        run.store(false);
        memoryMonitor.join();
        // DEBUG("Process used up to ", memUsedMax - memInitial, " Mbs of memory");
    }

    void monitor()
    {
        while (run) {
            memUsedMax = std::max(memUsedMax, TestUtils::getMemoryUsage());
            std::this_thread::sleep_for(std::chrono::microseconds (10));
        }
    }
};

// ./tests -p -l all --run_test=MemoryBenchmarking

BOOST_AUTO_TEST_SUITE(MemoryBenchmarking)

    BOOST_FIXTURE_TEST_CASE(LoadSnapshots, MemoryMonitor)
    {
        PricePointBook::OrderBook book {};

        constexpr size_t symbolsCount { 1'000 };
        const std::vector<std::string> symbols = TestUtils::getUniqueSymbols(symbolsCount);

        Types::Snapshot snapshot;
        Utilities::parseSnapshot(R"(../../test/data/snapshot_BNBBTC_4000.json)", snapshot);

        for (const std::string& symbol: symbols) {
            book.loadSnapshot(snapshot, symbol);
        }

        std::cout << "Process used up to " << memUsedMax - memInitial << " Mbs of memory" << std::endl;
    };

BOOST_AUTO_TEST_SUITE_END()
