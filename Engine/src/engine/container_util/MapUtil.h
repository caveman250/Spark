#pragma once

namespace se::util
{
    struct RetrieveKey
    {
        template <typename T>
        typename T::first_type operator()(T keyValuePair) const
        {
            return keyValuePair.first;
        }
    };

    template <typename T, typename Map>
    std::vector<T> ToKeyArray(const Map& map)
    {
        std::vector<T> ret;
        ret.reserve(map.size());
        std::transform(map.begin(), map.end(), std::back_inserter(ret), RetrieveKey());
        return ret;
    }
}
