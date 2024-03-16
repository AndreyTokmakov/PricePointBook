/**============================================================================
Name        : Utilities.cpp
Created on  : 16.03.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Utilities.cpp
============================================================================**/

#include "Utilities.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <charconv>

namespace
{
    constexpr size_t readBlockSize { 1024 };

    bool ReadFile2String(const std::filesystem::path &filePath,
                         std::string& dst)
    {
        if (std::ifstream file(filePath); file.is_open() && file.good())
        {
            file.seekg(0, std::ios_base::end);
            size_t fileSize = file.tellg(), bytesRead = 0;
            file.seekg(0, std::ios_base::beg);

            dst.resize(fileSize);
            while ((bytesRead += file.readsome(dst.data() + bytesRead, readBlockSize)) < fileSize) { }
            return true;
        }
        return false;
    }

    template<typename T>
    bool extractArray(const auto &json,
                      const std::string &key,
                      std::vector<T>& dest)
    {
        if (const auto iter = json.find(key); json.end() != iter && iter->is_array())
        {
            std::string value;
            for (const auto &orderInfo: iter.value())
            {
                if (orderInfo.is_array() && 2 == orderInfo.size() && orderInfo[0].is_string() && orderInfo[1].is_string())
                {
                    T& entry = dest.emplace_back();

                    orderInfo[0].get_to(value);
                    if (const auto [ptr, err] = std::from_chars(value.data(), value.data() + value.size(), entry.price);
                            err != std::errc{})
                        return false;

                    orderInfo[1].get_to(value);
                    if (const auto [ptr, err] = std::from_chars(value.data(), value.data() + value.size(), entry.quantity);
                            err != std::errc{})
                        return false;
                }
            }
        }

        dest.shrink_to_fit();
        return true;
    }
}

namespace Utilities
{
    using json = nlohmann::json;

    bool parseEvent(const std::string &eventJsonString,
                    Types::Event &event)
    {
        const json& eventJson { json::parse(eventJsonString) };

        if (const auto iter = eventJson.find("s"); eventJson.end() != iter && iter->is_string()) {
            event.symbol = iter.value();
        } else
            return false;

        if (const auto iter = eventJson.find("U"); eventJson.end() != iter && iter->is_number_integer()) {
            event.firstID = iter.value();
        } else
            return false;

        if (const auto iter = eventJson.find("u"); eventJson.end() != iter && iter->is_number_integer()) {
            event.lastID = iter.value();
        } else
            return false;

        if (!extractArray(eventJson, "b", event.buyOrders))
            return false;
        if (!extractArray(eventJson, "a", event.sellOrders))
            return false;

        return true;
    }

    void parseSnapshot(const std::filesystem::path& path,
                       Types::Snapshot& snapshot)
    {
        std::string jsonString;
        ReadFile2String(path, jsonString);
        const json& snapshotJson { json::parse(jsonString) };

        snapshot.asks.clear();
        snapshot.bids.clear();

        extractArray(snapshotJson, "bids", snapshot.bids);
        extractArray(snapshotJson, "asks", snapshot.asks);
    }

    Types::Event makeEvent(const std::string& symbol,
                           const std::vector<Order>& buyOrders,
                           const std::vector<Order>& sellOrders,
                           id_type firstID,
                           id_type lastID)
    {
        return { symbol, firstID, lastID, buyOrders, sellOrders };
    }
}