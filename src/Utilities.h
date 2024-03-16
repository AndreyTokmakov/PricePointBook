/**============================================================================
Name        : Utilities.h
Created on  : 16.03.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Utilities.h
============================================================================**/

#ifndef PRICEPOINTBOOK_UTILITIES_H
#define PRICEPOINTBOOK_UTILITIES_H

#include <filesystem>
#include "Types.h"

namespace Utilities
{
    using namespace Types;

    bool parseEvent(const std::string &jsonString,
                    Types::Event &event);

    void parseSnapshot(const std::filesystem::path& path,
                       Types::Snapshot &snapshot);

    [[maybe_unused]]
    Types::Event makeEvent(const std::string& symbol,
                           const std::vector<Order>& buyOrders = {},
                           const std::vector<Order>& sellOrders = {},
                           id_type firstID = 0,
                           id_type lastID = 0);
};

#endif //PRICEPOINTBOOK_UTILITIES_H
