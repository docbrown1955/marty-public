#include "multifunction.h"
#include "exceptions.h"

namespace sgl {

    AbstractMultiFunction::AbstractMultiFunction(GExpr const &arg)
        :m_argument({arg})
    {}
    AbstractMultiFunction::AbstractMultiFunction(std::vector<GExpr> const &arg)
        :m_argument(arg)
    {}

    bool AbstractMultiFunction::contains(csl::Index const &index) const
    {
        return std::any_of(
                m_argument.begin(),
                m_argument.end(),
                [&](GExpr const &arg) { return arg->contains(index); }
                );
    }
    size_t AbstractMultiFunction::size() const
    {
        return m_argument.size();
    }
    GExpr const &AbstractMultiFunction::argument(size_t i) const
    {
        return m_argument[i];
    }
    GExpr &AbstractMultiFunction::argument(size_t i)
    {
        return m_argument[i];
    }
    AbstractGammaSym::iterator AbstractMultiFunction::begin()
    {
        return m_argument.begin();
    }
    AbstractGammaSym::const_iterator AbstractMultiFunction::begin() const
    {
        return m_argument.begin();
    }
    AbstractGammaSym::iterator AbstractMultiFunction::end()
    {
        return m_argument.end();
    }
    AbstractGammaSym::const_iterator AbstractMultiFunction::end() const
    {
        return m_argument.end();
    }

    void AbstractMultiFunction::printArgs(std::ostream &out, char sep) const
    {
        for (size_t i = 0; i != m_argument.size(); ++i) {
            m_argument[i]->print(out);
            if (i < m_argument.size() - 1) {
                out << sep ;
            }
        }
    }

    size_t AbstractLiteral::size() const
    {
        return 0;
    }
    GExpr const &AbstractLiteral::argument(size_t) const
    {
        errorPrint();
        throw Exception::AbstractCall;
    }
    GExpr &AbstractLiteral::argument(size_t)
    {
        errorPrint();
        throw Exception::AbstractCall;
    }
    AbstractGammaSym::iterator AbstractLiteral::begin()
    {
        errorPrint();
        throw Exception::AbstractCall;
    }
    AbstractGammaSym::const_iterator AbstractLiteral::begin() const
    {
        errorPrint();
        throw Exception::AbstractCall;
    }
    AbstractGammaSym::iterator AbstractLiteral::end()
    {
        errorPrint();
        throw Exception::AbstractCall;
    }
    AbstractGammaSym::const_iterator AbstractLiteral::end() const
    {
        errorPrint();
        throw Exception::AbstractCall;
    }
}
