#pragma once

namespace se::util
{
    struct RetrieveKey
    {
        template <typename T>
        typename T::first_type operator()(const T& keyValuePair) const
        {
            return keyValuePair.first;
        }
    };

    struct RetrieveValue
    {
        template <typename T>
        typename T::second_type operator()(const T& keyValuePair) const
        {
            return keyValuePair.second;
        }
    };

    template <typename T, typename Map, typename Retriever>
    std::vector<T> ToArray(const Map& map)
    {
        std::vector<T> ret;
        ret.reserve(map.size());
        std::transform(map.begin(), map.end(), std::back_inserter(ret), Retriever());
        return ret;
    }

    template <typename T, typename Map>
    std::vector<T> ToKeyArray(const Map& map)
    {
        return ToArray<T, Map, RetrieveKey>(map);
    }

    template <typename T, typename Map>
    std::vector<T> ToValueArray(const Map& map)
    {
        return ToArray<T, Map, RetrieveValue>(map);
    }
}
