#pragma once

#include <cassert>
#include <iostream>

namespace csl {

/**
 * @brief Encapsulates a value of a given to ensure that it initialized when
 * used.
 *
 * @details An object of type InitSanitizer<T> can be used as a number for
 * assignment and will be automatically converted to the encapsulated value
 * when necessary. When assigned, the value becomes safe (initially not). When
 * the encapsulated value is taken from the object, it checks that it is indeed
 * initialized before returning it to ensure that there is no garbage result.
 *
 * @tparam T Type of the encapsulated object.
 */
template <typename T>
class InitSanitizer {

  public:
    constexpr InitSanitizer()
    {
    }

    constexpr InitSanitizer(char const t_name[]) : name(t_name)
    {
    }

    InitSanitizer(T const &t)
    {
        this->operator=(t);
    }

    InitSanitizer &operator=(T const &t)
    {
        m_safe  = true;
        m_value = t;
        return *this;
    }

    operator T() const
    {
        return get();
    }

    T get() const
    {
        if (!m_safe) {
            std::cerr << "Error: param \"" << name << "\" is used ";
            std::cerr << "uninitialized, please assign it a m_value using ";
            std::cerr << "standard m_value assignement.\n";
            assert(false);
        }
        return m_value;
    }

    void reset()
    {
        m_safe = false;
    }

    void print(std::ostream &out = std::cout) const
    {
        if (m_safe)
            out << name << " = " << m_value << '\n';
        else
            out << name << " uninitialized.\n";
    }

    friend std::ostream &operator<<(std::ostream &       out,
                                    InitSanitizer const &san)
    {
        out << san.m_value;
        return out;
    }

  public:
    char const *name;

  private:
    T    m_value;
    bool m_safe{false};
};

} // namespace csl
