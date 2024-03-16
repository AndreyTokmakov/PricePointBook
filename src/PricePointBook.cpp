/**============================================================================
Name        : PricePointBook.cpp
Created on  : 16.03.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : OrderBook.cpp
============================================================================**/

#include "PricePointBook.h"
#include "Utilities.h"
#include "Types.h"

#include <nlohmann/json.hpp>
#include <fstream>

namespace PricePointBook
{
    using namespace Types;
    using json = nlohmann::json;

    [[nodiscard]]
    std::optional<price_type> SymbolOrders::getBid() const noexcept
    {
        if (buyOrders.empty())
            return std::nullopt;
        return buyOrders.begin()->first;
    }

    [[nodiscard]]
    std::optional<price_type> SymbolOrders::getAsk() const noexcept
    {
        if (sellOrders.empty())
            return std::nullopt;
        return sellOrders.begin()->first;
    }
}

namespace PricePointBook
{
    template<typename Key,
            typename Value,
            std::predicate<Key, Key> Comparator>
    void OrderBook::updateOrder(const std::vector<Order>& eventOrders,
                                std::map<Key, Value, Comparator>& orders)
    {
        for (const Order& order: eventOrders)
        {
            if (0 == order.quantity) {
                /** Deleting order --> since quantity has become zero **/
                orders.erase(order.price);
            }
            else if (auto [iter, newOrderInserted] = orders.emplace(order.price, order.quantity);
                    false == newOrderInserted) {
                /** Updating the exising one **/
                iter->second = order.quantity;
            }
        }
    }

    void OrderBook::handleEvent(const Event& event)
    {
        SymbolOrders & ordersBySymbol = orderBook[event.symbol];

        updateOrder(event.buyOrders, ordersBySymbol.buyOrders);
        updateOrder(event.sellOrders, ordersBySymbol.sellOrders);
    }

    void OrderBook::loadSnapshot(const Types::Snapshot& snapshot,
                                 const std::string& symbol)
    {
        SymbolOrders & orders { orderBook[symbol] };
        for (const Order& priceInfo: snapshot.bids)
            orders.buyOrders.emplace(priceInfo.price, priceInfo.quantity);
        for (const Order& priceInfo: snapshot.asks)
            orders.sellOrders.emplace(priceInfo.price, priceInfo.quantity);
    }

    [[nodiscard]]
    std::optional<price_type> OrderBook::getBid(const std::string& symbol) const noexcept
    {
        if (const auto iter = orderBook.find(symbol); orderBook.end() != iter)
            return iter->second.getBid();
        return std::nullopt;
    }

    [[nodiscard]]
    std::optional<price_type> OrderBook::getAsk(const std::string& symbol) const noexcept
    {
        if (const auto iter = orderBook.find(symbol); orderBook.end() != iter)
            return iter->second.getAsk();
        return std::nullopt;
    }
}