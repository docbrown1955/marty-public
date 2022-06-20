#include "multipermutation.h"
#include <algorithm>
#include <iostream>

namespace csl {

MultiPermutation::MultiPermutation(
    std::vector<std::vector<size_t>> const &t_numbers)
{
    load(t_numbers);
}

void MultiPermutation::load(std::vector<std::vector<size_t>> const &t_numbers)
{
    size_t sz = 0;
    for (const auto &vec : t_numbers)
        sz += vec.size();
    m_numbers.reserve(sz);
    m_boundaries.reserve(t_numbers.size());
    for (const auto &vec : t_numbers) {
        m_boundaries.push_back(m_numbers.end());
        m_numbers.insert(m_numbers.end(), vec.begin(), vec.end());
    }
    m_boundaries.push_back(m_numbers.end());
    m_numbers_init = m_numbers;
}

bool MultiPermutation::nextPermutation()
{
    for (size_t i = 0; i + 1 != m_boundaries.size(); ++i) {
        if (std::next_permutation(m_boundaries[i], m_boundaries[i + 1]))
            return true;
    }
    return false;
}

void MultiPermutation::print() const
{
    for (size_t i = 0; i + 1 != m_boundaries.size(); ++i) {
        std::cout << "( ";
        for (auto iter = m_boundaries[i]; iter != m_boundaries[i + 1];
             ++iter) {
            std::cout << *iter << " ";
        }
        std::cout << ")" << '\n';
    }
}

} // namespace csl
