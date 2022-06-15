#pragma once

#include <vector>

namespace csl {

class MultiPermutation {

  public:
    MultiPermutation(std::vector<std::vector<size_t>> const &t_numbers);

    MultiPermutation(MultiPermutation const &other) = delete;
    MultiPermutation &operator=(MultiPermutation const &other) = delete;

    MultiPermutation(MultiPermutation &&other) = default;
    MultiPermutation &operator=(MultiPermutation &&other) = default;

    bool nextPermutation();

    template <class T>
    std::vector<T> applyPermutation(std::vector<T> const &collection)
    {
        std::vector<T> res(collection);
        for (size_t i = 0; i != m_numbers.size(); ++i) {
            assert(m_numbers_init[i] < collection.size()
                   && m_numbers[i] < collection.size());
            res[m_numbers_init[i]] = collection[m_numbers[i]];
        }
        return res;
    }

    void print() const;

  private:
    void load(std::vector<std::vector<size_t>> const &t_numbers);

  private:
    std::vector<size_t>                        m_numbers;
    std::vector<size_t>                        m_numbers_init;
    std::vector<std::vector<size_t>::iterator> m_boundaries;
};

} // namespace csl
