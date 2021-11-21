#include <csl>

int main() {

    ///////////////////////////////////////////////////
    /*************************************************/
    // Basic manipulations                           //
    /*************************************************/
    ///////////////////////////////////////////////////
    
    Expr a = csl::constant_s("a");
    Expr x = csl::variable_s("x");
    Expr t = csl::variable_s("t");

    Expr f = csl::prod_s(a, x);
    Expr res = csl::sum_s(
            csl::prod_s(
                csl::float_s(2.5), 
                csl::Derive(csl::prod_s(a, f), t)), 
            csl::int_s(4));
    std::cout << res << std::endl;

    ///////////////////////////////////////////////////
    // Using interface 
    ///////////////////////////////////////////////////

    f = a * x;
    res = 2.5 * csl::Derive(a * f, t) + 4;
    std::cout << res << std::endl;

    ///////////////////////////////////////////////////
    /*************************************************/
    // Going further, replacement and evaluation     //
    /*************************************************/
    ///////////////////////////////////////////////////

    res = csl::Derive(res, t);
    res = csl::Replace(res, x, csl::pow_s(t / a, 2));
    res = csl::Evaluate(res); // Evaluates the double derivative 
    std::cout << res << std::endl;

    ///////////////////////////////////////////////////
    /*************************************************/
    // Checking invariance explicitely, expansion    //
    /*************************************************/
    ///////////////////////////////////////////////////

    Expr y = csl::variable_s("y");
    Expr delta = x - y; // shortcut for (x-y) in the next
    res = csl::exp_s(delta*delta) - delta*csl::cos_s(delta);
    // res is invariant by replacing x -> x+a, y -> y+a
    // by expanding it, the invariance becomes less obvious
    Expand(res, true);
    res = Replace(res, x, x + a);
    res = Replace(res, y, y + a);
    std::cout << res << std::endl;

    // Derive wrt a and expanding gives 0. The result is 
    // indeed independent of a.
    res = Derive(res, a);
    Expand(res, true);
    std::cout << res << std::endl;

    return 0;
}
