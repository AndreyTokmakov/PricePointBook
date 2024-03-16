/**============================================================================
Name        : unit_tests.cpp
Created on  : 16.03.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description :
============================================================================ **/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE BasicCorectnessUnitTests

#include "Utilities.h"
#include "Types.h"
#include "PricePointBook.h"
#include "utils/TestUtils.h"

#include <nlohmann/json.hpp>

#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>

#include <fstream>
#include <random>
#include <unordered_set>

namespace PricePointBook
{
    using namespace PricePointBook;

    struct Tester
    {
        OrderBook& orderBook;
        Snapshot lastUpdated;

        explicit Tester(OrderBook& book): orderBook { book } {}

        void loadSnapshot(const std::filesystem::path& path,
                          const std::string& symbol)
        {
            // Test data init starts
            Utilities::parseSnapshot(path, lastUpdated);

            orderBook.loadSnapshot(lastUpdated, symbol);
        }

        [[nodiscard]]
        size_t getSymbolCount() const noexcept
        {
            return orderBook.orderBook.size();
        }

        [[nodiscard]]
        SymbolOrders& getOrdersBySymbol(const std::string& symbol) const noexcept
        {
            /** Its expected to crush if not order for symbol exits --> good for tests behaviour **/
            return orderBook.orderBook.at(symbol);
        }

        [[nodiscard]]
        Types::price_type getLastBid() const noexcept
        {
            Types::price_type minPrice { std::numeric_limits<Types::price_type>::min() };
            for (const Order& order: lastUpdated.bids)
                minPrice = std::max(minPrice, order.price);
            return minPrice;
        }

        [[nodiscard]]
        Types::price_type getLastAsk() const noexcept
        {
            Types::price_type maxPrice { std::numeric_limits<Types::price_type>::max() };
            for (const Order& order: lastUpdated.asks)
                maxPrice = std::min(maxPrice, order.price);
            return maxPrice;
        }
    };
}

using OrderBook = PricePointBook::OrderBook;
using Tester = PricePointBook::Tester;

struct BaseFixture
{
    OrderBook book {};
    Tester tester {book};

    static inline const std::string symbolDefault {"BNBBTC"};

    BaseFixture() = default;

    virtual ~BaseFixture() = default;
};


BOOST_FIXTURE_TEST_SUITE(SnapshotTests, BaseFixture)

    BOOST_AUTO_TEST_CASE(Load_100_TestBID)
    {
        tester.loadSnapshot(R"(../../test/data/snapshot_BNBBTC_1000.json)", symbolDefault);

        BOOST_REQUIRE_EQUAL(tester.getLastBid(), book.getBid(symbolDefault).value());
        BOOST_REQUIRE_EQUAL(1000, tester.getOrdersBySymbol(symbolDefault).buyOrders.size());
    };

    BOOST_AUTO_TEST_CASE(Load_4000_TestBID)
    {
        tester.loadSnapshot(R"(../../test/data/snapshot_BNBBTC_4000.json)", symbolDefault);

        BOOST_REQUIRE_EQUAL(tester.getLastBid(), book.getBid(symbolDefault).value());
        BOOST_REQUIRE_EQUAL(4000, tester.getOrdersBySymbol(symbolDefault).buyOrders.size());
    };

    BOOST_AUTO_TEST_CASE(Load_100_TestASK)
    {
        tester.loadSnapshot(R"(../../test/data/snapshot_BNBBTC_1000.json)", symbolDefault);

        BOOST_REQUIRE_EQUAL(tester.getLastAsk(), book.getAsk(symbolDefault).value());
        BOOST_REQUIRE_EQUAL(1000, tester.getOrdersBySymbol(symbolDefault).buyOrders.size());
    };

    BOOST_AUTO_TEST_CASE(Load_4000_TestASK)
    {
        tester.loadSnapshot(R"(../../test/data/snapshot_BNBBTC_4000.json)", symbolDefault);

        BOOST_REQUIRE_EQUAL(tester.getLastAsk(), book.getAsk(symbolDefault).value());
        BOOST_REQUIRE_EQUAL(4000, tester.getOrdersBySymbol(symbolDefault).buyOrders.size());
    };

    BOOST_AUTO_TEST_CASE(CheckLoad_100_Unique_Symbols)
    {
        constexpr size_t symbolsCount {100};
        const std::vector<std::string> symbols = TestUtils::getUniqueSymbols(symbolsCount);

        for (const std::string& symbol: symbols) {
            tester.loadSnapshot(R"(../../test/data/snapshot_100.json)", symbol);
        }

        BOOST_REQUIRE_EQUAL(symbolsCount, tester.getSymbolCount());
    };

    BOOST_AUTO_TEST_CASE(CheckLoad_10000_Unique_Symbols)
    {
        constexpr size_t symbolsCount { 10'000 };
        const std::vector<std::string> symbols = TestUtils::getUniqueSymbols(symbolsCount);

        for (const std::string& symbol: symbols) {
            tester.loadSnapshot(R"(../../test/data/snapshot_100.json)", symbol);
        }

        BOOST_REQUIRE_EQUAL(symbolsCount, tester.getSymbolCount());
    };

BOOST_AUTO_TEST_SUITE_END()


BOOST_FIXTURE_TEST_SUITE(HandleEventsTests, BaseFixture)

    BOOST_AUTO_TEST_CASE(HandleEvent_BID_HasToBeUpdated)
    {
        tester.loadSnapshot(R"(../../test/data/snapshot_100.json)", symbolDefault);

        const Types::price_type bidBefore = book.getBid(symbolDefault).value();
        const Types::price_type dstBid = bidBefore + 0.001f;

        Types::Event event = Utilities::makeEvent(symbolDefault,{{dstBid,5} },{});
        book.handleEvent(event);

        BOOST_REQUIRE_EQUAL(book.getBid(symbolDefault).value() , dstBid);
    }

    BOOST_AUTO_TEST_CASE(HandleEvent_ASK_HasToBeUpdated)
    {
        tester.loadSnapshot(R"(../../test/data/snapshot_100.json)", symbolDefault);

        const Types::price_type askBefore = book.getAsk(symbolDefault).value();
        const Types::price_type dstAsk = askBefore - 0.001f;

        Types::Event event = Utilities::makeEvent(symbolDefault,{},{{dstAsk,5}});
        book.handleEvent(event);

        BOOST_REQUIRE_EQUAL(book.getAsk(symbolDefault).value() , dstAsk);
    }

    BOOST_AUTO_TEST_CASE(HandleEvent_BID_HasToBeUpdated_MultipleSymbols)
    {
        const std::vector<std::string> symbols = TestUtils::getUniqueSymbols(10);

        Types::price_type testDelta = 0.001f;
        for (const std::string& symbol: symbols)
        {
            tester.loadSnapshot(R"(../../test/data/snapshot_100.json)", symbol);
            const Types::price_type bidBefore = book.getBid(symbol).value();

            testDelta += testDelta;
            const Types::price_type dstBid = bidBefore + testDelta;

            Types::Event event = Utilities::makeEvent(symbol,{{dstBid,5} },{});
            book.handleEvent(event);

            BOOST_REQUIRE_EQUAL(book.getBid(symbol).value() , dstBid);
        }
    }

    BOOST_AUTO_TEST_CASE(HandleEvent_ASK_HasToBeUpdated_MultipleSymbols)
    {
        const std::vector<std::string> symbols = TestUtils::getUniqueSymbols(10);

        Types::price_type testDelta = 0.001f;
        for (const std::string& symbol: symbols)
        {
            tester.loadSnapshot(R"(../../test/data/snapshot_100.json)", symbol);
            const Types::price_type askBefore = book.getAsk(symbol).value();

            testDelta += testDelta;
            const Types::price_type dstAsk = askBefore - testDelta;

            Types::Event event = Utilities::makeEvent(symbol,{},{{dstAsk,5} });
            book.handleEvent(event);

            BOOST_REQUIRE_EQUAL(book.getAsk(symbol).value() , dstAsk);
        }
    }

    BOOST_AUTO_TEST_CASE(HandleEvent_BID_Remove_ZeroQuantity)
    {
        tester.loadSnapshot(R"(../../test/data/snapshot_100.json)", symbolDefault);

        const Types::price_type bidBefore = book.getBid(symbolDefault).value();
        book.handleEvent(Utilities::makeEvent(symbolDefault,{{bidBefore,0} },{}));

        BOOST_REQUIRE_NE(book.getBid(symbolDefault).value() , bidBefore);
        BOOST_REQUIRE_LE(book.getBid(symbolDefault).value() , bidBefore);
    }

    BOOST_AUTO_TEST_CASE(HandleEvent_ASK_Remove_ZeroQuantity)
    {
        tester.loadSnapshot(R"(../../test/data/snapshot_100.json)", symbolDefault);

        const Types::price_type askBefore = book.getAsk(symbolDefault).value();
        book.handleEvent(Utilities::makeEvent(symbolDefault,{},{{askBefore, 0}}));

        BOOST_REQUIRE_NE(book.getAsk(symbolDefault).value() , askBefore);
        BOOST_REQUIRE_GE(book.getAsk(symbolDefault).value() , askBefore);
    }

BOOST_AUTO_TEST_SUITE_END()
