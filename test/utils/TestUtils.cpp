/**============================================================================
Name        : TestUtils.cpp
Created on  : 16.03.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : TestUtils.cpp
============================================================================**/

#include "TestUtils.h"
#include <fstream>
// #include <format>
#include <unordered_set>
#include <random>
#include <iostream>
#include <charconv>


namespace TestUtils
{
    constexpr uint16_t bytesInKB { 1024 };

    int32_t randomIntegerInRange(int32_t from, int32_t until) {
        std::random_device randomDevice{};
        std::mt19937 generator(randomDevice());
        return std::uniform_int_distribution<int>{from, until}(generator);
    }

    float randomFloatInRange(float from, float until)
    {
        std::random_device randomDevice{};
        std::mt19937 generator(randomDevice());
        return std::uniform_real_distribution<float>{from, until}(generator);
    }

    std::string randomString(size_t size)
    {
        std::random_device rd{};
        std::mt19937 generator = std::mt19937 {rd()};
        auto ud = std::uniform_int_distribution<> {(int)'a', (int)'z'};

        std::string str;
        str.reserve(size);
        while (size-- > 0)
            str.push_back(static_cast<char>(ud(generator)));
        return str;
    }

    std::vector<std::string> getUniqueSymbols(int size)
    {
        std::unordered_set<std::string> symbols;
        symbols.reserve(size);

        while (size > 0) {
            if (symbols.insert(randomString(6)).second)
                --size;
        }

        return {symbols.begin(), symbols.end() };
    }


    double getMemoryUsage()
    {
        double resultKb = 0;
        if (std::ifstream statusFile("/proc/self/status"); statusFile.is_open() && statusFile.good())
        {
            std::string line;
            while (std::getline(statusFile, line)) {
                if (line.find("VmRSS") != std::string::npos) {
                    resultKb = atof(line.data() + 6);
                    // std::from_chars(line.data() + 6, line.data() + line.size(), resultKb);
                    break;
                }
            };
        }
        return static_cast<double>(resultKb / bytesInKB);
    }

    // Creates from a Snapshot a set of Events consisting of 10/4 (by Default at least) prices present in the snapshot
    // (for searching and updating them) and prices partially absent in the snapshot - for adding them
    std::vector<Types::Event> generateTestEvents(const Types::Snapshot& snapshot,
                                                 const std::string& symbol,
                                                 uint32_t priceHitCount,
                                                 uint32_t priceMissCount,
                                                 uint32_t factor)
    {
        const size_t bundleSize { snapshot.asks.size() / factor };
        std::vector<Types::Event> testEvents;
        testEvents.reserve(bundleSize);

        const Types::price_type maxBuyPrice = snapshot.bids.front().price, minBuyPrice = snapshot.bids.back().price;
        const Types::price_type maxSellPrice = snapshot.asks.front().price, minSellPrice = snapshot.asks.back().price;

        for (size_t idx = 0; idx < bundleSize; ++idx)
        {
            Types::Event& event = testEvents.emplace_back(symbol);
            for (uint32_t n = 0; n < priceHitCount; ++n)
            {
                const int32_t randAskID = randomIntegerInRange(0, static_cast<int32_t>(snapshot.asks.size() - 1));
                event.sellOrders.push_back(snapshot.asks[randAskID]);

                const int32_t ranBidID = randomIntegerInRange(0, static_cast<int32_t>(snapshot.bids.size() - 1));
                event.buyOrders.push_back(snapshot.bids[ranBidID]);
            }

            for (uint32_t n = 0; n < priceMissCount; ++n)
            {
                /** This will generate a missing/new price levels or orders ... i hope ))) **/
                event.sellOrders.emplace_back(randomFloatInRange(minSellPrice, maxSellPrice), 1);
                event.buyOrders.emplace_back(randomFloatInRange(minBuyPrice, maxBuyPrice), 1);
            }
        }

        return testEvents;
    }
}