/**============================================================================
Name        : TestUtils.h
Created on  : 16.03.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : TestUtils.h
============================================================================**/

#ifndef PRICEPOINTBOOK_TESTUTILS_H
#define PRICEPOINTBOOK_TESTUTILS_H

#include "Types.h"

namespace TestUtils
{
    [[nodiscard("Do not forget to use the return value. Call could be expensive")]]
    double getMemoryUsage();

    [[nodiscard]]
    int32_t randomIntegerInRange(int32_t from = 0, int32_t until = 1000);

    [[nodiscard]]
    float randomFloatInRange(float from = 0, float until = 1);

    [[nodiscard("Do not forget to use the return value. Call could be expensive")]]
    std::string randomString(size_t size);

    [[nodiscard("Do not forget to use the return value. Call could be expensive")]]
    std::vector<std::string> getUniqueSymbols(int size = 10);

    [[nodiscard("Do not forget to use the return value. Call could be expensive")]]
    std::vector<Types::Event> generateTestEvents(const Types::Snapshot& snapshot,
                                                 const std::string& symbol,
                                                 uint32_t priceHitCount = 10,
                                                 uint32_t priceMissCount = 4,
                                                 uint32_t factor = 4);
}

#endif //PRICEPOINTBOOK_TESTUTILS_H
