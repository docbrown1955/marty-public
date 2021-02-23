#pragma once

#include <utility>
#include <vector>

namespace csl {

template<class Key, class Value> 
class linear_map {

public:
    using iterator = 
        typename std::vector<std::pair<Key, Value>>::iterator;
    using const_iterator = 
        typename std::vector<std::pair<Key, Value>>::const_iterator;

    auto size() const { return data.size(); }
    bool empty() const { return data.empty(); }
    auto begin()       { return data.begin(); }
    auto begin() const { return data.begin(); }
    auto end()       { return data.end(); }
    auto end() const { return data.end(); }
    auto cbegin() const { return data.begin(); }
    auto cend() const { return data.end(); }

    iterator erase(iterator iter) {
        size_t pos = std::distance(begin(), iter);
        data.erase(iter);
        return data.begin() + pos;
    }
    iterator find(Key const &value) {
        auto first = data.begin();
        const auto end = data.end();
        while (first != end) {
            if (first->first == value)
                break;
            ++first;
        }
        return first;
    }
    const_iterator find(Key const &value) const {
        auto first = data.begin();
        const auto end = data.end();
        while (first != end) {
            if (first->first == value)
                break;
            ++first;
        }
        return first;
    }

    Value &operator[](Key const &key) {
        auto pos = find(key);
        if (pos == end()) {
            data.push_back({key, Value()});
            return data.back().second;
        }
        return pos->second;
    }

private:

    std::vector<std::pair<Key, Value>> data;
};

}
