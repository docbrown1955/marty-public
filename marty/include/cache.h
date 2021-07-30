#pragma once

#include <functional>
#include <csl.h>

namespace mty {

template<class T, class U>
class Cache {

public:

    using CachedData = std::pair<T, U>;
    using Comparator = std::function<bool(T const&, T const&)>;
    using Releaser   = std::function<U(U const&)>;
    IMPLEMENTS_STD_VECTOR(CachedData, cached)

    explicit
    Cache(
            Comparator &&t_comparator = [](T const &a, T const &b) { return a == b; },
            Releaser   &&t_releaser   = [](U const &res) { return res; }
         )
        :comparator(std::forward<Comparator>(t_comparator)),
        releaser(std::forward<Releaser>(t_releaser))
    {

    }

    template<class Calculator>
    U calculate(T const &input, Calculator &&calc)
    {
        if (auto pos = find(input) ; pos != end()) {
            return releaser(pos->second);
        }
        return releaser(add(input, calc(input)));
    }

    auto find(T const &key) {
        return std::find_if(cached.begin(), cached.end(), 
                [&](CachedData const &data) {
                    return comparator(data.first, key);
                });
    }

    auto find(T const &key) const {
        return std::find_if(cached.begin(), cached.end(), 
                [&](CachedData const &data) {
                    return comparator(data.first, key);
                });
    }

private:

    U const &add(T const &key, U const &value) {
        if (auto pos = find(key) ; pos == end()) {
            cached.push_back({key, value});
            return cached.back().second;
        }
        else {
            return pos->second;
        }
    }

private:

    std::vector<CachedData> cached;
    Comparator comparator;
    Releaser   releaser;
};

} // namespace mty
