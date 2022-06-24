#pragma once

#include <array>
#include <cmath>
#include <cassert>
#include <iostream>

template<class Target, class First, class ...Args>
struct are_convertible_to {
    static constexpr bool value = 
        are_convertible_to<Target, First>::value
        && are_convertible_to<Target, Args...>::value;
};

template<class Target, class First>
struct are_convertible_to<Target, First> {
    static constexpr bool value = std::is_convertible<First, Target>::value;
};

template<class Target, class First, class ...Args>
inline constexpr bool are_convertible_to_v 
    = are_convertible_to<Target, First, Args...>::value;

template<std::size_t nIn, std::size_t nOut, typename float_t = double>
class Kinematics_base {

public:

    static constexpr auto nTot = nIn + nOut;
    using mass_container_t     = std::array<float_t, nTot>;
    using momenta_container_t  = std::array<float_t, (nTot * (nTot - 1)) / 2>;


protected:

    template<class ...Masses,
        typename = std::enable_if_t<are_convertible_to_v<float_t, Masses...>>
        >
    Kinematics_base(Masses &&...masses)
        :m_masses({masses...})
    {
        static_assert(nIn  > 0, "Needs at least one incoming particle !");
        static_assert(nOut > 0, "Needs at least one outgoing particle !");
        static_assert(sizeof...(Masses) == nTot, "Wrong number of masses.");
    }

public:

    float_t mass(const std::size_t i) const {
        assert(i < nTot);
        return m_masses[i];
    }

    float_t energy(const std::size_t i) const {
        assert(i < nTot);
        return m_energies[i];
    }

    float_t pSquared(const std::size_t i) const {
        assert(i < nTot);
        const auto E = energy(i);
        const auto m = mass(i);
        return E*E - m*m;
    }

    float_t lorentzInvariant(
            const std::size_t i, 
            const std::size_t j
            ) const {
        assert(i < nTot);
        assert(j < nTot);
        if (i == j) {
            const auto m = mass(i);
            return m*m;
        }
        else if (i > j) {
            return lorentzInvariant(j, i);
        }
        else {
            // Index taking into account only the upper triangular matrix
            // s_ij, j > i
            const std::size_t index = (nTot*i - (i*(i + 1))/2) + (j - i - 1);
            return m_squaredMomenta[index];
        }
    }

    bool isPhysical() const {
        for (std::size_t i = 0; i != nTot; ++i) 
            if (pSquared(i) < 0)
                return false;
        return true;
    }

    void print(std::ostream &out = std::cout) const
    {
        out << "Kinematics for " << nIn << " -> " << nOut << '\n';
        out << (isPhysical() ? "Physical !\n" : "Non physical !\n");
        out << "Energies (masses, momenta) :\n";
        for (std::size_t i = 0; i != nTot; ++i) {
            out << "E" << i << " = " << energy(i);
            out << " (m" << i << " = " << mass(i);
            out << ", p" << i << "^2 = " << pSquared(i) << ")" << '\n';
        }
        out << "squared momenta :\n";
        for (std::size_t i = 0; i != nTot; ++i) {
            for (std::size_t j = i+1; j != nTot; ++j) {
                out << "s_" << i << j << " = " << lorentzInvariant(i, j) << '\n';
            }
        }
    }

protected:

    void testAndWarnForNegativePSquared(float_t pSquared) const
    {
        if (pSquared < 0) {
            std::cerr << "Warning: negative momentum squared encountered in "
                << nIn << " -> " << nOut << " process : " << pSquared << ".\n";
        }
    }

protected:

    mass_container_t    m_masses;
    mass_container_t    m_energies;
    momenta_container_t m_squaredMomenta;
};

template<typename float_t>
inline float_t twoParticlesEnergySplit(
        float_t E,
        float_t mA,
        float_t mB
        )
{
    return (E*E + mA*mA - mB*mB) / (2*E);
}

template<typename float_t>
inline float_t momentum(
        float_t E,
        float_t m
        )
{
    return E*E - m*m;
}

template<std::size_t nIn, std::size_t nOut, typename float_t = double>
class Kinematics : public Kinematics_base<nIn, nOut, float_t> {};

template<typename float_t>
class Kinematics<1, 2, float_t> : public Kinematics_base<1, 2, float_t> {

public:

    Kinematics(
            float_t m0,
            float_t m1,
            float_t m2
            )
        :Kinematics_base<1, 2, float_t>(m0, m1, m2)
    {
        init();
    }

private:

    void init() 
    {
        const auto m0 = this->m_masses[0];
        const auto m1 = this->m_masses[1];
        const auto m2 = this->m_masses[2];

        const auto E1  = twoParticlesEnergySplit(m0, m1, m2);
        const auto E2  = twoParticlesEnergySplit(m0, m2, m1);
        const auto p_2 = momentum(E2, m1);

        this->m_energies       = { m0, E1, E2 };
        this->m_squaredMomenta = { 
            m0*E1,       // s_01
            m0*E2,       // s_02
            E1*E2 + p_2  // s_12
        };
    }
};

template<typename float_t>
class Kinematics<2, 1, float_t> : public Kinematics<1, 2, float_t> {

public:

    using Base      = Kinematics_base<1, 2, float_t>;
    using Delegatee = Kinematics     <1, 2, float_t>;

    Kinematics(
            float_t m0,
            float_t m1, 
            float_t m2
            )
        :Delegatee(m2, m0, m1)
    {
    }

private:

    static std::size_t effIndex(const std::size_t i) {
        assert(i < 3);
        return (i + 1) % 3;
    }
    
public:

    float_t mass(const std::size_t i) const {
        return Delegatee::mass(effIndex(i));
    }

    float_t energy(const std::size_t i) const {
        return Delegatee::energy(effIndex(i));
    }

    float_t pSquared(const std::size_t i) const {
        return Delegatee::pSquared(effIndex(i));
    }

    float_t lorentzInvariant(
            const std::size_t i, 
            const std::size_t j
            ) const {
        return Delegatee::lorentzInvariant(effIndex(i), effIndex(j));
    }

    void print(std::ostream &out = std::cout) const
    {
        out << "Kinematics for 2 -> 1" << '\n';
        out << (Base::isPhysical() ? "Physical !\n" : "Non physical !\n");
        out << "Energies (masses, momenta) :\n";
        for (std::size_t i = 0; i != 3; ++i) {
            const size_t iEff = effIndex(i);
            out << "E" << i << " = " << Delegatee::energy(iEff);
            out << " (m" << i << " = " << Delegatee::mass(iEff);
            out << ", p" << i << "^2 = " << Delegatee::pSquared(iEff) << ")" << '\n';
        }
        out << "squared momenta :\n";
        for (std::size_t i = 0; i != 3; ++i) {
            const std::size_t iEff = effIndex(i);
            for (std::size_t j = 0; j != 3; ++j) {
                const std::size_t jEff = effIndex(j);
                if (iEff < jEff) {
                    const std::size_t i1 = (i < j) ? i : j;
                    const std::size_t i2 = (i < j) ? j : i;
                    out << "s_" << i1 << i2 << " = ";
                    out << Delegatee::lorentzInvariant(iEff, jEff) << '\n';
                }
            }
        }
    }

};

/**
 * @brief Kinematics for general 2 -> 2 processes.
 *
 * @details Kinematics are defined the following way for four particles of 
 * mass respectively \f$ (m_0, m_1, m_2, m_3) \f$:
 * \f[
 *      \left(\begin{array}{c}E_1\\0\\0\\p\end{array}\right)
 *      +\left(\begin{array}{c}E_2\\0\\0\\-p\end{array}\right)
 *      =\left(\begin{array}{c}E_3\\p^\prime\sin\theta\cos\phi\\p^\prime\sin\theta\sin\phi\\p^\prime\cos\theta\end{array}\right)
 *      +\left(\begin{array}{c}E_4\\-p^\prime\sin\theta\cos\phi\\-p^\prime\sin\theta\sin\phi\\-p^\prime\cos\theta\end{array}\right),
 * \f]
 * with 
 * \f[
 *      E_0 + E_1 = E_2 + E_3 \equiv E_{CM},\\
 *      E_0^2 = m_0^2 + p^2,\\
 *      E_1^2 = m_1^2 + p^2,\\
 *      E_2^2 = m_2^2 + p^{\prime 2},\\
 *      E_3^2 = m_3^2 + p^{\prime 2}.\\
 * \f]
 * \f$ E_{CM} \f$ is the center of mass energy, \f$ \theta \f$ the angle between
 * the incoming and outgoing particle beems in the Center of Mass Frame (CMF).
 * The kinematics for 2 -> 2 processes are completely independent on \f$ \phi \f$,
 * that is why it is not considered in this class.
 *
 * @tparam float_t Precision for the kinematics object, default = double.
 */
template<typename float_t>
class Kinematics<2, 2, float_t> : public Kinematics_base<2, 2, float_t> {

public:

    Kinematics(
            float_t m0,
            float_t m1,
            float_t m2,
            float_t m3,
            float_t t_E_CM,
            float_t t_theta,
            bool    forward = true
            )
        :Kinematics_base<2, 2, float_t>(m0, m1, m2, m3),
        m_E_CM(t_E_CM),
        m_theta(t_theta)
    {
        init(forward);
    }

    float_t E_CM()  const { return m_E_CM;  }
    float_t theta() const { return m_theta; }

private:

    void init(bool forward) 
    {
        const auto m0 = this->m_masses[0];
        const auto m1 = this->m_masses[1];
        const auto m2 = this->m_masses[2];
        const auto m3 = this->m_masses[3];

        const auto E0   = twoParticlesEnergySplit(m_E_CM, m0, m1);
        const auto E1   = twoParticlesEnergySplit(m_E_CM, m1, m0);
        const auto E2   = twoParticlesEnergySplit(m_E_CM, m2, m3);
        const auto E3   = twoParticlesEnergySplit(m_E_CM, m3, m2);
        const auto pi_2 = momentum(E0, m0);
        const auto pf_2 = momentum(E2, m2);
        this->testAndWarnForNegativePSquared(pi_2);
        this->testAndWarnForNegativePSquared(pf_2);
        const auto crossProduct = (forward ? 1 : -1) * 
            std::sqrt(pi_2) * std::sqrt(pf_2) * std::cos(m_theta);

        this->m_energies       = { E0, E1, E2, E3 };
        this->m_squaredMomenta = { 
            E0*E1 + pi_2,          // s_01
            E0*E2 - crossProduct,  // s_02
            E0*E3 + crossProduct,  // s_03
            E1*E2 + crossProduct,  // s_12
            E1*E3 - crossProduct,  // s_13
            E2*E3 + pf_2           // s_23
        };
    }

protected:

    float_t m_E_CM;
    float_t m_theta;
};

