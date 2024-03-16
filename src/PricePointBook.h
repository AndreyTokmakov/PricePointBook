/**============================================================================
Name        : PricePointBook.h
Created on  : 16.03.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : PricePointBook.h
============================================================================**/

#ifndef PRICEPOINTBOOK_PRICEPOINTBOOK_H
#define PRICEPOINTBOOK_PRICEPOINTBOOK_H

#include <map>
#include <unordered_map>
#include <optional>
#include "Types.h"

namespace PricePointBook
{
    using namespace Types;

    /** To maintain ‘best price’ for each side (bids & asks) for each symbol separately **/
    struct SymbolOrders final
    {
        template<std::predicate<price_type, price_type> Comparator>
        using OrderList = std::map<price_type, quantity_type, Comparator>;

        /** Bids are ordered from the highest price to lowest **/
        OrderList<std::greater<>> buyOrders {};

        /** asks are ordered from the lowest price to highest. **/
        OrderList<std::less<>> sellOrders {};

        [[nodiscard]]
        std::optional<price_type> getBid() const noexcept;

        [[nodiscard]]
        std::optional<price_type> getAsk() const noexcept;
    };


    class OrderBook final
    {
        /** To have an O(1) Symbol <--> Event/Order matching **/
        std::unordered_map<std::string, SymbolOrders> orderBook;

    public:

        void handleEvent(const Event& event);

        void loadSnapshot(const Types::Snapshot& snapshot,
                          const std::string& symbol);

        [[nodiscard]]
        std::optional<price_type> getBid(const std::string& symbol) const noexcept;

        [[nodiscard]]
        std::optional<price_type> getAsk(const std::string& symbol) const noexcept;

    private:

        template<typename Key, typename Value,
                std::predicate<Key, Key> Comparator>
        void updateOrder(const std::vector<Order>& eventOrders,
                         std::map<Key, Value, Comparator>& orders);

        /** For the testing purposes **/
        friend struct Tester;
    };
};

#endif //PRICEPOINTBOOK_PRICEPOINTBOOK_H
