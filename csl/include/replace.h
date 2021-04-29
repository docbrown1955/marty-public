#pragma once

#include "abstract.h"
#include "index.h"

namespace csl {

///////////////////////////////////////////////////
// Expression replacements
///////////////////////////////////////////////////

void Replace(
        csl::Expr       &expr,
        csl::Expr const &from,
        csl::Expr const &to
        );

void Replace(
        csl::Expr         &expr,
        csl::Parent const &from,
        csl::Parent const &to
        );

void Replace(
        csl::Expr         &expr,
        csl::Parent const &from,
        csl::Expr   const &to
        );

void Replace(
        csl::Expr                      &expr,
        std::vector<csl::Parent> const &from,
        std::vector<csl::Parent> const &to
        );

void Replace(
        csl::Expr                    &expr,
        std::vector<csl::Expr> const &from,
        std::vector<csl::Expr> const &to
        );

void ReplaceIndicial(
        csl::Expr                    &expr,
        std::vector<csl::Expr> const &from,
        std::vector<csl::Expr> const &to
        );

void Replace(
        csl::Expr                      &expr,
        std::vector<csl::Parent> const &from,
        std::vector<csl::Expr>   const &to,
        bool                            refresh = true
        );

template<class T, class U, class Type, class Type2 = Type>
struct canDecay {
    static inline constexpr bool value = 
        std::is_convertible_v<std::decay_t<T>, Type>
        && std::is_convertible_v<std::decay_t<U>, Type2>;

};

template<class T, class U, class Type, class Type2 = Type>
inline constexpr bool canDecay_v = canDecay<T, U, Type, Type2>::value;

// Template non void Replaced instead of Replace
// All impossible overloads are defined explicitly to allow other templates 
// overloads to work. Sad that C++20 is too recent (concepts would have been
// much more relevant) than this solution with std::enable_if.
template<class T, class U, class ...Params,
        typename = std::enable_if_t<
                !csl::canDecay_v<T, U, csl::Index>
                && !csl::canDecay_v<T, U, csl::IndexStructure>
                && !csl::canDecay_v<T, U, std::vector<csl::Index>>
                >
        >
[[nodiscard]] 
csl::Expr Replaced(
        csl::Expr const &expr,
        T              &&p1,
        U              &&p2,
        Params         &&...params
        )
{
    csl::Expr cpy = expr;
    csl::Replace(
        cpy, 
        std::forward<T>(p1),
        std::forward<U>(p2),
        std::forward<Params>(params)...
        );
    return cpy;
}

///////////////////////////////////////////////////
// Index replacements
///////////////////////////////////////////////////

[[nodiscard]]
csl::Expr Replaced(
        csl::Expr  const &expr,
        csl::Index const &from,
        csl::Index const &to,
        bool              refresh = true
        );

inline void Replace(
        csl::Expr        &expr,
        csl::Index const &from,
        csl::Index const &to,
        bool              refresh = true
        ) 
{
    expr = csl::Replaced(expr, from, to, refresh);
}

[[nodiscard]] 
csl::Expr Replaced(
        csl::Expr               const &expr,
        std::vector<csl::Index> const &from,
        std::vector<csl::Index> const &to,
        bool                           refresh = true
        );

inline void Replace(
        csl::Expr                     &expr,
        std::vector<csl::Index> const &from,
        std::vector<csl::Index> const &to,
        bool                           refresh = true
        )
{
    expr = csl::Replaced(expr, from, to, refresh);
}

[[nodiscard]] 
csl::Expr Replaced(
        csl::Expr           const &expr,
        csl::IndexStructure const &from,
        csl::IndexStructure const &to,
        bool                       refresh = true
        );

inline void Replace(
        csl::Expr                 &expr,
        csl::IndexStructure const &from,
        csl::IndexStructure const &to,
        bool                       refresh = true
        )
{
    expr = csl::Replaced(expr, from, to, refresh);
}

///////////////////////////////////////////////////
// Abbreviation treatment
///////////////////////////////////////////////////

/**
 * @brief Replacement rule prototype.
 *
 * @details The function takes a constant expression as parameter and a boolean.
 * If true, this boolean specifies that the rule is used only as a predicate for
 * the moment i.e. that no calculation must be done in the return value (the 
 * optional will be used as a bool to know if a replacement must take place).
 * If false, this function must return the result of the replacement in the 
 * std::optional<csl::Expr>. If no replacement is possible, return std::nullopt.
 */
using replacementRule = 
    std::function<std::optional<csl::Expr>(csl::Expr const&, bool)>;

/**
 * @brief Converts a replacementRule into a predicate.
 *
 * @param rule The replacement rule to convert.
 *
 * @return A lambda expression that calls the rule with a the "predicate" 
 * specification (meaning that the rule should not calculate the result of the
 * replacement) and simply returns the boolean value of the returned optional.
 */
inline auto ruleToPredicate(replacementRule const &rule) {
    return [&rule](csl::Expr const &expr) -> bool {
        return bool(rule(expr, true));
    };
}

/**
 * @brief Search for a true value for a given predicate in an expression, 
 * considering also sub-expressions encapsulated by abbreviations.
 *
 * @param expr      The expression in which the search is done.
 * @param predicate Boolean predicate.
 *
 * @return \b True  if on sub-expression checks the predicate.
 * @return \b False else.
 */
bool hasWeakDependency(
        csl::Expr const &expr,
        std::function<bool(csl::Expr const&)> const &predicate
        );

/**
 * @brief Applies a replacement rule thoughout an expression, also entering 
 * abbreviations and replacing them if a replacement is found.
 *
 * @param expr Expression in which the replacement takes place.
 * @param rule Rule for the replacement.
 */
void applyThroughAbbreviations(
        csl::Expr &expr,
        replacementRule const &rule
        );

///////////////////////////////////////////////////
// Replacement rules
///////////////////////////////////////////////////

std::optional<csl::Expr> scalarReplacement(
        csl::Expr              const &expr,
        std::vector<csl::Expr> const &from,
        std::vector<csl::Expr> const &ccFrom,
        std::vector<csl::Expr> const &to,
        bool                          isPredicate
        );

std::optional<csl::Expr> indicialReplacement(
        csl::Expr                     const &expr,
        std::vector<csl::Expr>        const &from,
        std::vector<csl::Parent_info> const &parentFrom,
        std::vector<csl::Expr>        const &to,
        bool                                 isPredicate
        );

std::optional<csl::Expr> tensorReplacement(
        csl::Expr                const &expr,
        std::vector<csl::Parent> const &from,
        std::vector<csl::Parent> const &to,
        bool                            isPredicate
        );

std::optional<csl::Expr> tensorExpressionReplacement(
        csl::Expr                const &expr,
        std::vector<csl::Parent> const &from,
        std::vector<csl::Expr>   const &to,
        bool                            isPredicate
        );

///////////////////////////////////////////////////
// Helper functions for indicial replacements
///////////////////////////////////////////////////

void ResetDummyIndices(csl::Expr &expr);

void ApplyIndices(
        csl::Expr                 &expr,
        csl::IndexStructure const &structure
        );

void ApplyIndices(
        csl::Expr                 &expr,
        csl::IndexStructure const &from,
        csl::IndexStructure const &to
        );

void renameIndex(
        csl::Index                       &index,
        std::map<csl::Index, csl::Index> &mapping
        );

void RenameIndices(csl::Expr& expr);

csl::Expr RenamedIndices(csl::Expr const& expr);

void internal_RenameIndices(
        csl::Expr                        &expr,
        std::map<csl::Index, csl::Index> &mapping
        );

///////////////////////////////////////////////////
// Template helper overloads
///////////////////////////////////////////////////

template<class ParentType,
    typename = std::enable_if_t<std::is_convertible_v<ParentType, Parent>>
    >
inline void Replace(
        Expr& expr,
        std::vector<ParentType> const& t_from,
        std::vector<ParentType> const& t_to)
{
    std::vector<Parent> from(t_from.size());
    std::vector<Parent> to(t_to.size());
    for (size_t i = 0; i != from.size(); ++i)
        from[i] = t_from[i];
    for (size_t i = 0; i != to.size(); ++i)
        to[i] = t_to[i];
    Replace(expr, from, to);
}

template<class ParentType,
    typename = std::enable_if_t<std::is_convertible_v<ParentType, Parent>>
    >
inline void Replace(
        Expr& expr,
        std::vector<ParentType> const& t_from,
        std::vector<csl::Expr>  const& to)
{
    std::vector<Parent> from(t_from.size());
    for (size_t i = 0; i != from.size(); ++i)
        from[i] = t_from[i];
    Replace(expr, from, to);
}

///////////////////////////////////////////////////
// Functions that try a replacement and keep the
// result only if it is simpler than the initial
// expression
///////////////////////////////////////////////////

template<class ...Args>
Expr Tried(const Expr& expr, Args &&...args)
{
    Expr trial = Replaced(expr, std::forward<Args>(args)...);
    return (*trial < expr.get()) ? trial : expr;
}

template<class ...Args>
void Try(Expr& expr, Args &&...args)
{
    Expr trial = Replaced(expr, std::forward<Args>(args)...);
    if (trial < expr)
        expr = trial;
}

} // namespace csl
