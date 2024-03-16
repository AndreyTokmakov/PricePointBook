/**============================================================================
Name        : Types.h
Created on  : 16.03.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Types
============================================================================**/

#ifndef PRICEPOINTBOOK_TYPES_H
#define PRICEPOINTBOOK_TYPES_H

#include <cstdint>
#include <string>
#include <vector>

namespace Types
{
    using id_type = uint32_t;
    using quantity_type = float;
    using price_type = float;

    template <typename T1, typename T2> requires
            std::is_arithmetic_v<T1> && std::is_arithmetic_v<T2>
    struct NamedPair
    {
        T1 price {};
        T2 quantity {};
    };

    using Order = NamedPair<price_type, quantity_type>;

    struct Event
    {
        /** ignoring the field in the context of this task: outside the scope **/
        // id_type timestamp {};

        std::string symbol {};

        /** ignoring the field in the context of this task: outside the scope **/
        id_type firstID { 0 };

        /** ignoring the field in the context of this task: outside the scope **/
        id_type lastID { 0 };

        std::vector<Order> buyOrders {};
        std::vector<Order> sellOrders {};
    };

    struct Snapshot
    {
        /** ignoring the field in the context of this task: outside the scope **/
        // uint32_t lastUpdateId {0};

        std::vector<Order> bids;
        std::vector<Order> asks;
    };
}

#endif //PRICEPOINTBOOK_TYPES_H