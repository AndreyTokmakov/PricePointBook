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
}