// This file is part of MARTY.
//
// MARTY is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// MARTY is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with MARTY. If not, see <https://www.gnu.org/licenses/>.

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
