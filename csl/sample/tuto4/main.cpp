#include <csl>

using csl::Expr;

struct Condition {

    Expr expr;
    std::string spec;

    void print() const { std::cout << expr << spec << ".\n"; }
};

void applyInverse(
        Expr const &init, // sub-expression to inverse
        Expr const &variable, 
        Expr       &lhs, // left-hand side to modify for inversion
        std::vector<Condition> &conditions // Conditions to fill
        )
{
    csl::Type type = csl::GetType(init);
    switch(type) {

        case csl::Type::Cos: 
            conditions.push_back({lhs, " in [-1, 1]"});
            lhs = csl::acos_s(lhs);
            break;
        case csl::Type::ACos: 
            lhs = csl::cos_s(lhs);
            break;

        case csl::Type::Sum:
        case csl::Type::Prod:
            {
            bool dependencyFound = false;
            for (size_t i = 0; i != init->size(); ++i) {
                Expr argument = csl::GetArgument(init, i);
                if (csl::DependsExplicitlyOn(argument, variable)) {
                    if (dependencyFound) {
                        std::cerr << "Error: Double dependency not possible.\n";
                        return;
                    }
                }
                else {
                    if (type == csl::Type::Sum)
                        lhs = lhs - argument;
                    else {
                        lhs = lhs / argument;
                        conditions.push_back({argument, " != 0"});
                    }
                }
            }
            break;
            }

        default:
            break;
    }
}

std::vector<Condition> invert(
        Expr const &expr,
        Expr const &variable,
        Expr &lhs
        )
{
    std::vector<Condition> conditions;
    csl::VisitEachNodeCut(expr, 
    [&](Expr const &node)
    {
        if (not csl::DependsExplicitlyOn(node, variable))
            // Stop digging
            return true;

        applyInverse(node, variable, lhs, conditions);
        return false;
    });

    return conditions;
}

int main() {

    Expr A = csl::constant_s("A");
    Expr T = csl::constant_s("T");
    Expr t = csl::variable_s("t");

    Expr x = A * (1 + csl::cos_s(2* CSL_PI * t / T));
    std::cout << "x = " << x << std::endl;

    Expr target = csl::variable_s("x");
    auto conditions = invert(x, t, target);
    std::cout << "t = " << target << std::endl;
    for (const auto& cond : conditions)
        cond.print();

    return 0;
}
