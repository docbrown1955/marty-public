// Python binding of the symbolic expression facade (csl::api::ExprHandle).

#include "csl/api/error.h"
#include "csl/api/expr.h"
#include "csl/api/functions.h"
#include "csl/api/tensor.h"
#include "csl/api/transform.h"
#include "csl/enum.h"
#include "symbolic_bridge.h"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <cstddef>
#include <limits>
#include <optional>
#include <string>
#include <vector>

namespace py = pybind11;

namespace {

using csl::api::ExprHandle;
using csl::api::Number;
using csl::api::NumberKind;
using csl::api::Shape;
using csl::api::Structure;

/// Exact `int` value of a Python object, or `std::nullopt` if it is not an
/// integer or does not fit in `long long`.
std::optional<long long> exactInteger(py::handle integer)
{
    long long const value = PyLong_AsLongLong(integer.ptr());
    if (PyErr_Occurred()) {
        PyErr_Clear();
        return std::nullopt;
    }
    return value;
}

/// Coerces a Python object to an expression, or `std::nullopt` if it is not
/// supported. Used by operators, so that Python can try the reflected
/// operation or raise `TypeError`.
std::optional<ExprHandle> tryAsExpr(py::handle value)
{
    if (py::isinstance<ExprHandle>(value)) {
        return value.cast<ExprHandle const &>();
    }
    if (py::isinstance<py::bool_>(value) or py::isinstance<py::str>(value)) {
        return std::nullopt;
    }
    if (py::hasattr(value, "__marty_expr__")) {
        py::object converted = value.attr("__marty_expr__")();
        if (py::isinstance<ExprHandle>(converted)) {
            return converted.cast<ExprHandle const &>();
        }
        return std::nullopt;
    }
    if (PyIndex_Check(value.ptr())) {
        PyObject *raw = PyNumber_Index(value.ptr());
        if (raw == nullptr) {
            PyErr_Clear();
            return std::nullopt;
        }
        py::object index   = py::reinterpret_steal<py::object>(raw);
        auto const integer = exactInteger(index);
        if (!integer) {
            return std::nullopt;
        }
        return ExprHandle::integer(*integer);
    }
    if (py::hasattr(value, "numerator")
        and py::hasattr(value, "denominator")) {
        py::object numerator   = value.attr("numerator");
        py::object denominator = value.attr("denominator");
        auto const num         = exactInteger(numerator);
        auto const den         = exactInteger(denominator);
        if (!num or !den or *den == 0) {
            return std::nullopt;
        }
        return ExprHandle::rational(*num, *den);
    }
    if (PyComplex_Check(value.ptr()) or py::hasattr(value, "__complex__")) {
        Py_complex const z = PyComplex_AsCComplex(value.ptr());
        if (PyErr_Occurred()) {
            PyErr_Clear();
            return std::nullopt;
        }
        if (z.imag != 0.0) {
            return ExprHandle::complex(ExprHandle::real(z.real),
                                       ExprHandle::real(z.imag));
        }
        return ExprHandle::real(z.real);
    }
    if (py::hasattr(value, "__float__")) {
        double const d = PyFloat_AsDouble(value.ptr());
        if (PyErr_Occurred()) {
            PyErr_Clear();
            return std::nullopt;
        }
        return ExprHandle::real(d);
    }
    return std::nullopt;
}

/// Coerces the argument of `Expr(value)` and of the functions. Raises
/// `TypeError` for `bool` and `str`, and `ValueError` for any other value that
/// cannot be converted.
ExprHandle coerce(py::handle value)
{
    if (py::isinstance<py::bool_>(value) or py::isinstance<py::str>(value)) {
        throw py::type_error("Expr does not accept bool or str values");
    }
    if (auto converted = tryAsExpr(value)) {
        return *converted;
    }
    throw py::value_error("cannot convert value to Expr");
}

/// Python value of a number. Equal numbers give equal Python values (and so
/// equal hashes).
py::object numberValue(Number const &number)
{
    if (number.kind == NumberKind::Integer) {
        return py::int_(number.num);
    }
    if (number.kind == NumberKind::Rational) {
        return py::module_::import("fractions")
            .attr("Fraction")(number.num, number.den);
    }
    if (number.kind == NumberKind::Real) {
        return py::float_(static_cast<double>(number.real));
    }
    return py::reinterpret_steal<py::object>(PyComplex_FromDoubles(
        static_cast<double>(number.real), static_cast<double>(number.imag)));
}

/// Hashable key of a node: order independent for sums and products, numbers by
/// value, symbols by parent identity.
py::object structuralKey(Structure const &node)
{
    if (node.shape == Shape::Number) {
        return py::make_tuple("number", numberValue(*node.number));
    }
    if (node.shape == Shape::Symbol) {
        return py::make_tuple(
            "symbol",
            static_cast<unsigned long long>(node.expr->symbolIdentity()));
    }
    if (node.shape == Shape::Opaque) {
        return py::make_tuple("opaque", node.type);
    }
    std::vector<py::object> keys;
    keys.reserve(node.children.size());
    for (auto const &child : node.children) {
        keys.push_back(structuralKey(child));
    }
    if (node.shape == Shape::Ordered) {
        py::tuple children(keys.size());
        for (std::size_t i = 0; i < keys.size(); ++i) {
            children[i] = keys[i];
        }
        return py::make_tuple("ordered", node.type, children);
    }
    py::list list;
    for (auto const &key : keys) {
        list.append(key);
    }
    py::object counter
        = py::module_::import("collections").attr("Counter")(list);
    py::object multiset = py::module_::import("builtins")
                              .attr("frozenset")(counter.attr("items")());
    return py::make_tuple("unordered", node.type, multiset);
}

/// Applies a binary operation after coercing both operands. Returns
/// `NotImplemented` when an operand is not supported, so that Python can try
/// the reflected operation.
template <class Op>
py::object binary(py::handle lhs, py::handle rhs, Op op)
{
    auto const left  = tryAsExpr(lhs);
    auto const right = tryAsExpr(rhs);
    if (!left or !right) {
        return py::reinterpret_borrow<py::object>(Py_NotImplemented);
    }
    return py::cast(op(*left, *right));
}

template <ExprHandle (*Function)(ExprHandle const &)>
void defineFunction(py::module_ &m, char const *name, char const *doc)
{
    m.def(
        name,
        [](py::handle e) { return Function(coerce(e)); },
        py::arg("e"),
        doc);
}

/// Real part of an exact numerical value, as a double.
double realOf(Number const &number)
{
    switch (number.kind) {
    case NumberKind::Integer:
        return static_cast<double>(number.num);
    case NumberKind::Rational:
        return static_cast<double>(number.num)
               / static_cast<double>(number.den);
    case NumberKind::Real:
    case NumberKind::Complex:
        return static_cast<double>(number.real);
    }
    return 0.0;
}

/// Imaginary part of an exact numerical value, as a double.
double imagOf(Number const &number)
{
    return number.kind == NumberKind::Complex
               ? static_cast<double>(number.imag)
               : 0.0;
}

/// Python `complex` of a numerical value.
py::object complexValue(Number const &number)
{
    return py::reinterpret_steal<py::object>(
        PyComplex_FromDoubles(realOf(number), imagOf(number)));
}

/// Substitutions of a mapping, in the order of its items. Keys and values are
/// coerced to expressions.
std::vector<csl::api::Substitution> substitutions(py::handle mapping)
{
    if (!py::hasattr(mapping, "items")) {
        throw py::type_error("expected a mapping of Expr to Expr or number");
    }
    std::vector<csl::api::Substitution> result;
    py::object                          items = mapping.attr("items")();
    for (py::handle item : items) {
        py::tuple  pair  = py::reinterpret_borrow<py::tuple>(item);
        py::object key   = pair[0];
        py::object value = pair[1];
        result.emplace_back(coerce(key), coerce(value));
    }
    return result;
}

/// Named values of the keyword arguments of `evaluate`.
std::vector<csl::api::NamedValue> namedValues(py::kwargs const &kwargs)
{
    std::vector<csl::api::NamedValue> result;
    for (auto const &item : kwargs) {
        result.emplace_back(item.first.cast<std::string>(),
                            coerce(item.second));
    }
    return result;
}

/// Adds the transformation, printing and numeric methods to `Expr`.
void bindTransforms(py::class_<ExprHandle> &expr)
{
    expr.def(
            "expand",
            [](ExprHandle const &self, bool full) {
                return csl::api::expanded(self, full);
            },
            py::kw_only(),
            py::arg("full") = true,
            R"doc(Expansion of products and integer powers of sums. Returns a new expression.

``full`` is a keyword-only flag, default True. With ``full=False`` only the
top-level products are expanded and ``(x + y)**2`` is left unchanged.

>>> x = variable("x")
>>> y = variable("y")
>>> print(((x + y) ** 2).expand())
x^2 + 2*x*y + y^2
>>> print(((x + y) ** 2).expand(full=False))
(x + y)^2
)doc")
        .def(
            "copy",
            [](ExprHandle const &self, bool deep) {
                return csl::api::copied(self, deep);
            },
            py::kw_only(),
            py::arg("deep") = false,
            R"doc(A new expression equal to this one. Returns a new expression.

``deep`` is a keyword-only flag, default False. With ``deep=True`` the
sub-expressions are copied too.

>>> x = variable("x")
>>> y = variable("y")
>>> e = x + y
>>> print(e.copy(deep=True))
x + y
)doc")
        .def(
            "refresh",
            [](ExprHandle const &self, bool deep) {
                return csl::api::refreshed(self, deep);
            },
            py::kw_only(),
            py::arg("deep") = false,
            R"doc(A new expression with its canonical form restored. Returns a new expression.

``deep`` is a keyword-only flag, default False. With ``deep=True`` the
sub-expressions are refreshed too.

>>> x = variable("x")
>>> y = variable("y")
>>> print((x * y).refresh(deep=True))
x*y
)doc")
        .def(
            "factor",
            [](ExprHandle const &self, py::object const &by, bool full) {
                if (by.is_none()) {
                    return csl::api::factored(self, full);
                }
                return csl::api::factoredBy(self, coerce(by), full);
            },
            py::arg("by") = py::none(),
            py::kw_only(),
            py::arg("full") = false,
            R"doc(Factors the common factors out of sums. Returns a new expression.

With ``by``, factors out that expression instead. ``full`` is a keyword-only
flag, default False, passed on to the CSL transformation.

>>> x = variable("x")
>>> y = variable("y")
>>> print((x * y + x).factor())
x*(1 + y)
>>> print((x * y + x).factor(by=x))
(1 + y)*x
)doc")
        .def(
            "collect",
            [](ExprHandle const &self, py::iterable factors, bool full) {
                std::vector<ExprHandle> list;
                for (py::handle factor : factors) {
                    list.push_back(coerce(factor));
                }
                return csl::api::collected(self, list, full);
            },
            py::arg("factors"),
            py::kw_only(),
            py::arg("full") = false,
            R"doc(Groups the terms of a sum by the given factors. Returns a new expression.

``full`` is a keyword-only flag, default False, passed on to the CSL
transformation.

>>> a = variable("a")
>>> b = variable("b")
>>> x = variable("x")
>>> y = variable("y")
>>> (a * x + b * x + y).collect([x]) == (a + b) * x + y
True
)doc")
        .def(
            "diff",
            [](ExprHandle const &self, ExprHandle const &symbol) {
                return csl::api::derived(self, symbol);
            },
            py::arg("symbol"),
            R"doc(Derivative with respect to a symbol. Returns a new expression.

>>> x = variable("x")
>>> (x**3).diff(x) == 3 * x**2
True
)doc")
        .def(
            "replace",
            [](ExprHandle const &self, py::handle mapping) {
                return csl::api::replaced(self, substitutions(mapping));
            },
            py::arg("mapping"),
            R"doc(Replaces each key of the mapping by its value. Returns a new expression.

Keys and values are expressions or numbers. The replacement is a single
bottom-up pass: a substituted value is not visited again.

>>> x = variable("x")
>>> y = variable("y")
>>> (x + 2 * y).replace({x: y, y: x}) == 2 * x + y
True
>>> (x * y).replace({x: y, y: 2}) == 2 * y
True
)doc")
        .def(
            "evaluate",
            [](ExprHandle const &self, py::args args, py::kwargs kwargs) {
                if (args.size() > 1) {
                    throw py::type_error("evaluate takes at most one "
                                         "positional mapping");
                }
                csl::api::EvalMode mode = csl::api::EvalMode::All;
                py::kwargs         names;
                for (auto const &item : kwargs) {
                    if (item.first.cast<std::string>() == "mode") {
                        if (!py::isinstance<csl::api::EvalMode>(item.second)) {
                            throw py::type_error("evaluate: mode must be an "
                                                 "EvalMode");
                        }
                        mode = item.second.cast<csl::api::EvalMode>();
                    }
                    else {
                        names[item.first] = item.second;
                    }
                }
                std::vector<csl::api::Substitution> mapping;
                if (args.size() == 1 && !args[0].is_none()) {
                    mapping = substitutions(args[0]);
                }
                ExprHandle result = csl::api::evaluated(self, mapping, mode);
                if (!names.empty()) {
                    result = csl::api::evaluatedByName(
                        result, namedValues(names), mode);
                }
                return result;
            },
            R"doc(Replaces symbols by values, then evaluates the operations selected by mode.
Returns a new expression. Symbols that are not given are kept.

Takes an optional mapping, as for replace(), and keyword arguments that give
the values of symbols by name. The keyword ``mode`` (an EvalMode, default
``EvalMode.All``) selects the operations: with ``EvalMode.All`` the abbreviations
are expanded too. A symbol named ``mode`` cannot be given by keyword.

>>> x = variable("x")
>>> (x**2 + 1).evaluate({x: 3}) == 10
True
>>> (x**2 + 1).evaluate(x=3) == 10
True
>>> y = variable("y")
>>> (x * y).evaluate(x=2) == 2 * y
True
)doc")
        .def(
            "conjugate",
            [](ExprHandle const &self) { return csl::api::conjugated(self); },
            R"doc(Complex conjugate. Symbols are real: the conjugate of a variable is itself.

>>> x = variable("x")
>>> x.conjugate() == x
True
>>> I.conjugate() == -I
True
)doc")
        .def(
            "real_part",
            [](ExprHandle const &self) { return csl::api::realPart(self); },
            R"doc(Real part. Symbols are real.

>>> x = variable("x")
>>> (x + 2 * I).real_part() == x
True
)doc")
        .def(
            "imag_part",
            [](ExprHandle const &self) { return csl::api::imagPart(self); },
            R"doc(Imaginary part.

>>> x = variable("x")
>>> (x + 2 * I).imag_part() == 2
True
)doc")
        .def(
            "depends_on",
            [](ExprHandle const &self, ExprHandle const &symbol) {
                return csl::api::dependsOn(self, symbol);
            },
            py::arg("symbol"),
            R"doc(True if the expression depends on the symbol.

>>> x = variable("x")
>>> y = variable("y")
>>> (x + y).depends_on(x)
True
>>> c = constant("c")
>>> (x + y).depends_on(c)
False
)doc")
        .def_property_readonly(
            "free_symbols",
            [](ExprHandle const &self) {
                py::list list;
                for (auto const &symbol : csl::api::freeSymbols(self)) {
                    list.append(py::cast(symbol));
                }
                return py::module_::import("builtins").attr("frozenset")(list);
            },
            R"doc(Frozen set of the constants and variables of the expression.

>>> x = variable("x")
>>> y = variable("y")
>>> (x + y).free_symbols == frozenset({x, y})
True
)doc")
        .def_property_readonly(
            "name",
            [](ExprHandle const &self) { return csl::api::symbolName(self); },
            R"doc(Name of a constant or a variable. MartyTypeError for other expressions.

>>> variable("x").name
'x'
)doc")
        .def("__str__",
             [](ExprHandle const &self) {
                 std::string output = csl::api::text(self);
                 output.erase(output.find_last_not_of('\n') + 1);
                 return output;
             })
        .def("__repr__",
             [](ExprHandle const &self) {
                 std::string output = csl::api::text(self);
                 output.erase(output.find_last_not_of('\n') + 1);
                 return "<Expr: " + output + ">";
             })
        .def(
            "to_latex",
            [](ExprHandle const &self) { return csl::api::latex(self); },
            R"doc(LaTeX code of the expression, without math delimiters.

>>> x = variable("x")
>>> y = variable("y")
>>> print((x / y).to_latex())
\frac{x}{y}
)doc")
        .def("_repr_latex_",
             [](ExprHandle const &self) {
                 return "$" + csl::api::latex(self) + "$";
             })
        .def("__complex__",
             [](ExprHandle const &self) {
                 return complexValue(csl::api::numericValue(self));
             })
        .def("__float__",
             [](ExprHandle const &self) {
                 csl::api::Number const number = csl::api::numericValue(self);
                 if (imagOf(number) != 0.0) {
                     throw py::value_error("cannot convert a complex number "
                                           "to float");
                 }
                 return realOf(number);
             })
        .def("__int__", [](ExprHandle const &self) -> py::int_ {
            csl::api::Number const number = csl::api::numericValue(self);
            if (number.kind != NumberKind::Integer) {
                throw py::value_error("expression is not an integer");
            }
            return py::int_(number.num);
        });
}

} // namespace

// Spaces, indices and tensors. Python `Index` objects keep the Python `Space`
// they come from, so that `Index.space` is the same object.

namespace {

using csl::api::IndexHandle;
using csl::api::IndexRenamingHandle;
using csl::api::IndexType;
using csl::api::SpaceHandle;
using csl::api::SymmetrySpec;
using csl::api::TensorDerivativeHandle;
using csl::api::TensorFieldHandle;
using csl::api::TensorHandle;

/// Python `Index`: an index handle and the Python `Space` it belongs to.
/// `space` is `None` for the indices of an expression, which keep no owner.
struct IndexWrapper {
    IndexHandle handle;
    py::object  space;
};

/// Handle of an `Index` argument. Raises `TypeError` for other objects.
IndexHandle indexHandle(py::handle value)
{
    if (!py::isinstance<IndexWrapper>(value)) {
        throw csl::api::TypeError("expected an Index");
    }
    return value.cast<IndexWrapper const &>().handle;
}

/// Tuple of `Index` objects, all with the same Python `space`.
py::tuple exprTuple(std::vector<ExprHandle> const &exprs)
{
    py::tuple result(exprs.size());
    for (std::size_t i = 0; i < exprs.size(); ++i) {
        result[i] = py::cast(exprs[i]);
    }
    return result;
}

py::tuple indexTuple(std::vector<IndexHandle> const &indices,
                     py::object const               &space)
{
    py::tuple result(indices.size());
    for (std::size_t i = 0; i < indices.size(); ++i) {
        result[i] = py::cast(IndexWrapper{indices[i], space});
    }
    return result;
}

/// Renaming argument: a `dict` of free `Index` to free `Index`. Raises
/// `TypeError` for other objects and `ValueError` for dummy indices. Its
/// entries are read as presets by relabel_indices().
IndexRenamingHandle renamingArgument(py::handle value)
{
    if (!py::isinstance<py::dict>(value)) {
        throw csl::api::TypeError("renaming must be a dict of Index to Index");
    }
    IndexRenamingHandle renaming;
    for (auto const &item : py::reinterpret_borrow<py::dict>(value)) {
        renaming.add(indexHandle(item.first), indexHandle(item.second));
    }
    return renaming;
}

/// Adds the pairs of a renaming to the `dict` it was read from. Existing keys
/// keep their Python objects, only their values change.
void storeRenaming(py::dict &dict, IndexRenamingHandle const &renaming)
{
    for (auto const &[old, fresh] : renaming.pairs()) {
        dict[py::cast(IndexWrapper{old, py::none()})]
            = py::cast(IndexWrapper{fresh, py::none()});
    }
}

/// `str` argument. Raises `TypeError` for other objects.
std::string stringArgument(py::handle value, char const *what)
{
    if (!py::isinstance<py::str>(value)) {
        throw csl::api::TypeError(what);
    }
    return value.cast<std::string>();
}

/// Optional name argument: `None` or a `str`.
std::optional<std::string> optionalName(py::handle value, char const *what)
{
    if (value.is_none()) {
        return std::nullopt;
    }
    return stringArgument(value, what);
}

/// `int` argument, `bool` excluded. Raises `TypeError` for other objects and
/// `ValueError` when the value does not fit in a C++ `long long`.
long long integerArgument(py::handle value, char const *what)
{
    if (!py::isinstance<py::int_>(value) or py::isinstance<py::bool_>(value)) {
        throw csl::api::TypeError(what);
    }
    try {
        return value.cast<long long>();
    }
    catch (py::cast_error const &) {
        throw csl::api::ValueError("integer argument out of range");
    }
}

/// `int` argument, range-checked. Same errors as `integerArgument`.
int intArgument(py::handle value, char const *what)
{
    long long const number = integerArgument(value, what);
    if (number < std::numeric_limits<int>::min()
        or number > std::numeric_limits<int>::max()) {
        throw csl::api::ValueError("integer argument out of range");
    }
    return static_cast<int>(number);
}

/// Sequence of `str`. Raises `TypeError` for other objects.
std::vector<std::string> nameList(py::handle value, char const *what)
{
    if (!py::isinstance<py::iterable>(value)
        or py::isinstance<py::str>(value)) {
        throw csl::api::TypeError(what);
    }
    std::vector<std::string> names;
    for (py::handle item : value) {
        names.push_back(stringArgument(item, what));
    }
    return names;
}

/// Sequence of `Space`. Raises `TypeError` for other objects.
std::vector<SpaceHandle> spaceList(py::handle value)
{
    char const *what = "spaces must be a sequence of Space";
    if (!py::isinstance<py::iterable>(value)
        or py::isinstance<py::str>(value)) {
        throw csl::api::TypeError(what);
    }
    std::vector<SpaceHandle> spaces;
    for (py::handle item : value) {
        if (!py::isinstance<SpaceHandle>(item)) {
            throw csl::api::TypeError(what);
        }
        spaces.push_back(item.cast<SpaceHandle const &>());
    }
    return spaces;
}

/// Position of a symmetric index, from a Python `int`.
std::size_t symmetryPosition(py::handle value)
{
    long long const position
        = integerArgument(value, "symmetry position must be an int");
    if (position < 0) {
        throw csl::api::ValueError("symmetry position must not be "
                                   "negative");
    }
    return static_cast<std::size_t>(position);
}

/// Symmetry from a bool (full symmetry) or from a sequence of position
/// pairs.
SymmetrySpec symmetrySpec(py::handle spec)
{
    SymmetrySpec result;
    if (py::isinstance<py::bool_>(spec)) {
        result.fully = spec.cast<bool>();
        return result;
    }
    if (!py::isinstance<py::iterable>(spec)) {
        throw csl::api::TypeError("symmetry must be a bool or a "
                                  "sequence of "
                                  "position pairs");
    }
    for (py::handle item : spec) {
        if (!py::isinstance<py::sequence>(item) or py::len(item) != 2) {
            throw csl::api::TypeError("symmetry pair must be a "
                                      "(position, "
                                      "position) pair");
        }
        py::object const first  = item.cast<py::sequence>()[0];
        py::object const second = item.cast<py::sequence>()[1];
        result.pairs.emplace_back(symmetryPosition(first),
                                  symmetryPosition(second));
    }
    return result;
}

/// Adds the `Index`, `Space` and `Tensor` classes and the tensor
/// functions.
void bindTensorFacade(py::module_ &m)
{
    py::tuple const defaultIndexNames = py::make_tuple("i", "j", "k", "l");
    m.attr("DEFAULT_INDEX_NAMES")     = defaultIndexNames;

    py::enum_<IndexType>(
        m, "IndexType", "Type of an index: Free, Dummy or Fixed.")
        .value("Free", IndexType::Free)
        .value("Dummy", IndexType::Dummy)
        .value("Fixed", IndexType::Fixed);

    py::enum_<csl::PrimaryType>(m,
                                "PrimaryType",
                                "Primary type of an expression: a family of "
                                "expression types, such as "
                                "MultiFunction for Sum, Prod and Pow. Mirrors "
                                "csl::PrimaryType.")
        .value("Arbitrary", csl::PrimaryType::Arbitrary)
        .value("Numerical", csl::PrimaryType::Numerical)
        .value("Literal", csl::PrimaryType::Literal)
        .value("MultiFunction", csl::PrimaryType::MultiFunction)
        .value("ScalarFunction", csl::PrimaryType::ScalarFunction)
        .value("Vectorial", csl::PrimaryType::Vectorial)
        .value("Indicial", csl::PrimaryType::Indicial)
        .value("Field", csl::PrimaryType::Field);

    py::enum_<csl::Type>(m,
                         "ExprType",
                         "Type of an expression, such as Sum, Prod, Variable "
                         "or Integer. "
                         "Mirrors csl::Type.")
        .value("InheritanceType", csl::Type::InheritanceType)
        .value("NoType", csl::Type::NoType)
        .value("Integer", csl::Type::Integer)
        .value("Float", csl::Type::Float)
        .value("IntFraction", csl::Type::IntFraction)
        .value("IntFactorial", csl::Type::IntFactorial)
        .value("Complex", csl::Type::Complex)
        .value("NumericalEval", csl::Type::NumericalEval)
        .value("Imaginary", csl::Type::Imaginary)
        .value("Constant", csl::Type::Constant)
        .value("Variable", csl::Type::Variable)
        .value("Exp", csl::Type::Exp)
        .value("Log", csl::Type::Log)
        .value("DiracDelta", csl::Type::DiracDelta)
        .value("Abs", csl::Type::Abs)
        .value("Cos", csl::Type::Cos)
        .value("Sin", csl::Type::Sin)
        .value("Tan", csl::Type::Tan)
        .value("Cosh", csl::Type::Cosh)
        .value("Sinh", csl::Type::Sinh)
        .value("Tanh", csl::Type::Tanh)
        .value("ACos", csl::Type::ACos)
        .value("ASin", csl::Type::ASin)
        .value("ATan", csl::Type::ATan)
        .value("ACosh", csl::Type::ACosh)
        .value("ASinh", csl::Type::ASinh)
        .value("ATanh", csl::Type::ATanh)
        .value("Scalar", csl::Type::Scalar)
        .value("RealPart", csl::Type::RealPart)
        .value("ImaginaryPart", csl::Type::ImaginaryPart)
        .value("BooleanOperator", csl::Type::BooleanOperator)
        .value("TensorElement", csl::Type::TensorElement)
        .value("ScalarField", csl::Type::ScalarField)
        .value("TensorFieldElement", csl::Type::TensorFieldElement)
        .value("TDerivativeElement", csl::Type::TDerivativeElement)
        .value("StandardDuo", csl::Type::StandardDuo)
        .value("StandardMult", csl::Type::StandardMult)
        .value("Pow", csl::Type::Pow)
        .value("Prod", csl::Type::Prod)
        .value("Sum", csl::Type::Sum)
        .value("Polynomial", csl::Type::Polynomial)
        .value("Derivative", csl::Type::Derivative)
        .value("Commutator", csl::Type::Commutator)
        .value("Angle", csl::Type::Angle)
        .value("Factorial", csl::Type::Factorial)
        .value("Integral", csl::Type::Integral)
        .value("ScalarIntegral", csl::Type::ScalarIntegral)
        .value("VectorIntegral", csl::Type::VectorIntegral)
        .value("Vector", csl::Type::Vector)
        .value("Matrix", csl::Type::Matrix)
        .value("HighDTensor", csl::Type::HighDTensor);

    py::enum_<csl::api::EvalMode>(m,
                                  "EvalMode",
                                  "Evaluation mode of evaluate(): the "
                                  "operations it performs. Modes combine "
                                  "with | (for example EvalMode.Literal | "
                                  "EvalMode.Numerical). All is the "
                                  "union of the other modes and the default.")
        .value("Base", csl::api::EvalMode::Base)
        .value("Literal", csl::api::EvalMode::Literal)
        .value("Numerical", csl::api::EvalMode::Numerical)
        .value("Indicial", csl::api::EvalMode::Indicial)
        .value("Abbreviation", csl::api::EvalMode::Abbreviation)
        .value("All", csl::api::EvalMode::All)
        .def(
            "__or__",
            [](csl::api::EvalMode a, csl::api::EvalMode b) { return a | b; },
            py::is_operator())
        .def(
            "__and__",
            [](csl::api::EvalMode a, csl::api::EvalMode b) { return a & b; },
            py::is_operator());

    py::class_<IndexWrapper>(m,
                             "Index",
                             R"doc(Index of a space: free, dummy or fixed.

Create indices with Space.index(), Space.indices() and Space.fixed_index().
Two indices are equal only when they are the same index: two calls to index()
give two different indices.

>>> sp = Space("S", 4)
>>> i = sp.index("mu")
>>> i == i
True
>>> i == sp.index("mu")
False
)doc")
        .def_property_readonly(
            "name",
            [](IndexWrapper const &self) { return self.handle.name(); },
            R"doc(Name of a free or dummy index. MartyTypeError for a fixed index.)doc")
        .def_property_readonly(
            "space",
            [](IndexWrapper const &self) { return self.space; },
            R"doc(Space of the index, or None for an index of an expression.

>>> sp = Space("S", 4)
>>> sp.index().space is sp
True
)doc")
        .def(
            "__invert__",
            [](IndexWrapper const &self) {
                return IndexWrapper{self.handle.flipped(), self.space};
            },
            R"doc(Index with the opposite sign, written ``~i``.

>>> i = Space("S", 4).index()
>>> ~i == i.flipped()
True
)doc")
        .def(
            "flipped",
            [](IndexWrapper const &self) {
                return IndexWrapper{self.handle.flipped(), self.space};
            },
            R"doc(Index with the opposite sign.

>>> i = Space("S", 4).index()
>>> i.flipped().flipped() == i
True
)doc")
        .def_property_readonly(
            "type",
            [](IndexWrapper const &self) { return self.handle.type(); },
            R"doc(Type of the index, an IndexType. ValueError for an index of an expression.)doc")
        .def_property_readonly(
            "id",
            [](IndexWrapper const &self) { return self.handle.id(); },
            R"doc(CSL identifier of the index, an int. ValueError for an index of an expression.)doc")
        .def_property_readonly(
            "value",
            [](IndexWrapper const &self) { return self.handle.value(); },
            R"doc(Value of a fixed index, in 0..dimension-1. MartyTypeError for a free or dummy index.

>>> Space("S", 4).fixed_index(2).value
2
)doc")
        .def("__eq__",
             [](IndexWrapper const &self, py::handle other) -> py::object {
                 if (!py::isinstance<IndexWrapper>(other)) {
                     return py::reinterpret_borrow<py::object>(
                         Py_NotImplemented);
                 }
                 return py::bool_(self.handle.equals(indexHandle(other)));
             })
        .def("__hash__", [](IndexWrapper const &self) {
            auto const key = self.handle.hashKey();
            return py::hash(
                py::make_tuple(key.space, key.type, key.magnitude, key.id));
        });

    py::class_<SpaceHandle>(m,
                            "Space",
                            R"doc(Index space of a given dimension.

The predefined spaces are Minkowski, Euclid_R2, Euclid_R3 and Euclid_R4.

>>> Space("E4", 4).dimension
4
)doc")
        .def(py::init([](py::object const &name,
                         py::object const &dimension,
                         py::object const &indexNames) {
                 auto const names = nameList(indexNames,
                                             "Space: index_names must "
                                             "be a "
                                             "sequence of str");
                 if (names.empty()) {
                     throw csl::api::ValueError("Space: index_names "
                                                "must not "
                                                "be empty");
                 }
                 return SpaceHandle(
                     stringArgument(name, "Space: name must be a str"),
                     intArgument(dimension, "Space: dimension must be an int"),
                     names);
             }),
             py::arg("name"),
             py::arg("dimension"),
             py::arg("index_names") = defaultIndexNames,
             R"doc(Space(name, dimension, index_names=('i', 'j', 'k', 'l')).

name is a str. dimension is an int of at least 1. index_names is a non-empty
sequence of str: index() and indices() without a name take their default names
from it, in order.

>>> sp = Space("S", 4, index_names=("a", "b"))
>>> sp.index().name
'a'
>>> sp.index().name
'b'
)doc")
        .def_property_readonly(
            "dimension", &SpaceHandle::dimension, "Dimension of the space.")
        .def_property_readonly(
            "name", &SpaceHandle::name, "Name of the space.")
        .def(
            "index",
            [](py::object const &self, py::object const &name) {
                SpaceHandle const &space = self.cast<SpaceHandle const &>();
                auto const         given = optionalName(name,
                                                "index: name must be "
                                                        "a str or None");
                IndexHandle        index
                    = given ? space.index(*given) : space.index();
                return IndexWrapper{index, self};
            },
            py::arg("name") = py::none(),
            R"doc(Free index. With a name, the index has that name; otherwise it takes the next
default name of the space.

>>> Space("S", 4).index("nu").name
'nu'
)doc")
        .def(
            "indices",
            [](py::object const &self,
               py::object const &count,
               py::object const &name) {
                long long const number = integerArgument(count,
                                                         "indices: n "
                                                         "must be an "
                                                         "int");
                if (number < 0) {
                    throw csl::api::ValueError("indices: count must "
                                               "not be "
                                               "negative");
                }
                auto const         given = optionalName(name,
                                                "indices: name must "
                                                        "be a str "
                                                        "or None");
                SpaceHandle const &space = self.cast<SpaceHandle const &>();
                auto const         size  = static_cast<std::size_t>(number);
                return indexTuple(given ? space.indices(size, *given)
                                        : space.indices(size),
                                  self);
            },
            py::arg("n"),
            py::arg("name") = py::none(),
            R"doc(Tuple of n distinct free indices, named name when given, otherwise with
default names.

>>> a, b = Space("S", 4).indices(2)
>>> a == b
False
)doc")
        .def(
            "fixed_index",
            [](py::object const &self, py::object const &value) {
                return IndexWrapper{
                    self.cast<SpaceHandle const &>().fixedIndex(
                        intArgument(value,
                                    "fixed_index: value must be an "
                                    "int")),
                    self};
            },
            py::arg("value"),
            R"doc(Fixed index with the given value, in 0..dimension-1.)doc");

    m.attr("Minkowski") = py::cast(SpaceHandle::minkowski());
    m.attr("Euclid_R2") = py::cast(SpaceHandle::euclid2());
    m.attr("Euclid_R3") = py::cast(SpaceHandle::euclid3());
    m.attr("Euclid_R4") = py::cast(SpaceHandle::euclid4());

    py::class_<TensorHandle>(
        m,
        "Tensor",
        R"doc(Indexed tensor, with one space per index position. Create it with tensor().

Call it with one index per position to get an element. Each index must belong
to the space of its position.

>>> sp = Space("S", 4)
>>> T = tensor("T", [sp, sp], symmetric=True)
>>> i, j = sp.index("i"), sp.index("j")
>>> T(i, j) == T(j, i)
True
)doc")
        .def_property_readonly(
            "name", &TensorHandle::name, "Name of the tensor.")
        .def_property_readonly("spaces",
                               &TensorHandle::spaces,
                               "Index spaces of the tensor, one per index "
                               "position.")
        .def_property_readonly("fully_symmetric",
                               &TensorHandle::fullySymmetric,
                               "True if all index positions are symmetric.")
        .def_property_readonly("fully_antisymmetric",
                               &TensorHandle::fullyAntiSymmetric,
                               "True if all index positions are "
                               "antisymmetric.")
        .def(
            "__call__",
            [](TensorHandle const &self, py::args args) {
                std::vector<IndexHandle> indices;
                for (py::handle item : args) {
                    indices.push_back(indexHandle(item));
                }
                return self(indices);
            },
            "Element of the tensor, with one index per space.");

    m.def(
        "tensor",
        [](py::object const    &name,
           py::object const    &spaces,
           py::object const    &symmetric,
           py::object const    &antisymmetric,
           csl::ComplexProperty complexProperty) {
            return TensorHandle(
                stringArgument(name, "tensor: name must be a str"),
                spaceList(spaces),
                symmetrySpec(symmetric),
                symmetrySpec(antisymmetric),
                complexProperty);
        },
        py::arg("name"),
        py::arg("spaces"),
        py::kw_only(),
        py::arg("symmetric")        = false,
        py::arg("antisymmetric")    = false,
        py::arg("complex_property") = csl::ComplexProperty::Real,
        R"doc(Tensor with one space per index position. Call it with indices to get an
element.

symmetric and antisymmetric give the symmetries of the tensor: True for full
symmetry, or a sequence of position pairs, such as [(0, 1)], for the symmetry
between two positions. complex_property is the complex property of the tensor,
Real by default.

>>> sp = Space("S", 4)
>>> A = tensor("A", [sp, sp], antisymmetric=[(0, 1)])
>>> i, j = sp.index("i"), sp.index("j")
>>> A(i, j) == -A(j, i)
True
)doc");

    m.def(
        "diagonal_tensor",
        [](std::string const             &name,
           SpaceHandle const             &space,
           std::vector<ExprHandle> const &diagonal) {
            return diagonalTensor(name, space, diagonal);
        },
        py::arg("name"),
        py::arg("space"),
        py::arg("diagonal"),
        R"doc(Square tensor on a space, with the given diagonal and zero off-diagonal
entries. Call it with two indices of the space to get an element.

Unlike a sum of Kronecker deltas, the core sees this tensor as a numerical
diagonal matrix, as in the C++ models.
)doc");

    py::class_<TensorFieldHandle>(
        m,
        "TensorField",
        R"doc(Tensor field: a tensor that depends on a point tensor. Create it with
tensor_field().

Call it with one index per space of the field, then the point, a rank-1 Tensor
in the point space, to get an element.
)doc")
        .def_property_readonly(
            "name", &TensorFieldHandle::name, "Name of the field.")
        .def_property_readonly("point_space",
                               &TensorFieldHandle::pointSpace,
                               "Space of the point the field depends on.")
        .def_property_readonly("index_spaces",
                               &TensorFieldHandle::indexSpaces,
                               "Index spaces of the field, one per index "
                               "position.")
        .def_property_readonly(
            "primary_type",
            &TensorFieldHandle::primaryType,
            R"doc(Primary type of the field, as a PrimaryType (always PrimaryType.Field).

>>> sp = Space("S", 4)
>>> tensor_field("F", sp, [sp]).primary_type == PrimaryType.Field
True
)doc")
        .def(
            "__call__",
            [](TensorFieldHandle const &self, py::args args) {
                std::vector<py::handle> items;
                for (py::handle item : args) {
                    items.push_back(item);
                }
                if (items.empty()
                    or !py::isinstance<TensorHandle>(items.back())) {
                    throw csl::api::TypeError("tensor field element: the last "
                                              "argument must be a "
                                              "Tensor point");
                }
                std::vector<IndexHandle> indices;
                for (std::size_t i = 0; i + 1 < items.size(); ++i) {
                    indices.push_back(indexHandle(items[i]));
                }
                return self(indices,
                            items.back().cast<TensorHandle const &>());
            },
            "Element of the field at a point, with one index per space, then "
            "the point.");
    m.def(
        "tensor_field",
        [](py::object const &name,
           py::object const &pointSpace,
           py::object const &spaces) {
            if (!py::isinstance<SpaceHandle>(pointSpace)) {
                throw csl::api::TypeError("tensor_field: point_space must be "
                                          "a Space");
            }
            return TensorFieldHandle(
                stringArgument(name, "tensor_field: name must be a str"),
                pointSpace.cast<SpaceHandle const &>(),
                spaceList(spaces));
        },
        py::arg("name"),
        py::arg("point_space"),
        py::arg("spaces") = py::list(),
        R"doc(Tensor field, depending on a point tensor in point_space. spaces gives
the spaces of the field indices, in order; [] by default.

>>> sp = Space("S", 4)
>>> X = tensor("X", [sp])
>>> F = tensor_field("F", sp, [sp])
>>> [index.name for index in F(sp.index("i"), X).free_indices]
['i']
)doc");

    py::class_<TensorDerivativeHandle>(
        m,
        "TensorDerivative",
        R"doc(Derivative with respect to a point tensor. Create it with
tensor_derivative().

Call it with an index and the point tensor to get the derivative element. Apply
it to an expression with Expr.__call__ to differentiate that expression.
)doc")
        .def(
            "__call__",
            [](TensorDerivativeHandle const &self,
               py::handle                    index,
               py::handle                    point) {
                if (!py::isinstance<TensorHandle>(point)) {
                    throw csl::api::TypeError("tensor derivative element: the "
                                              "point must be a "
                                              "Tensor");
                }
                return self(indexHandle(index),
                            point.cast<TensorHandle const &>());
            },
            py::arg("index"),
            py::arg("point"),
            R"doc(Derivative element with respect to the point, with a free or dummy index.)doc");
    m.def(
        "tensor_derivative",
        [](py::object const &name, py::object const &space) {
            if (!py::isinstance<SpaceHandle>(space)) {
                throw csl::api::TypeError("tensor_derivative: space must be a "
                                          "Space");
            }
            return TensorDerivativeHandle(
                stringArgument(name, "tensor_derivative: name must be a str"),
                space.cast<SpaceHandle const &>());
        },
        py::arg("name"),
        py::arg("space"),
        R"doc(Tensor derivative with respect to a point tensor in space.)doc");
    m.def(
        "delta",
        [](py::handle i, py::handle j) {
            return csl::api::delta(indexHandle(i), indexHandle(j));
        },
        py::arg("i"),
        py::arg("j"),
        R"doc(Kronecker delta of two indices of the same space.)doc");
    m.def(
        "metric",
        [](py::handle i, py::handle j) {
            return csl::api::metric(indexHandle(i), indexHandle(j));
        },
        py::arg("i"),
        py::arg("j"),
        R"doc(Metric tensor of two indices of the same space.)doc");
    m.def(
        "epsilon",
        [](py::args args) {
            std::vector<IndexHandle> indices;
            for (py::handle item : args) {
                indices.push_back(indexHandle(item));
            }
            return csl::api::epsilon(indices);
        },
        R"doc(Levi-Civita symbol: one index per dimension of the space. Works for
dimensions 2 to 4.)doc");
}

} // namespace

csl::Index coreIndex(py::handle value)
{
    return indexHandle(value).raw();
}

py::object pyIndex(csl::Index const &index)
{
    return py::cast(IndexWrapper{IndexHandle(nullptr, index), py::none()});
}

void bindSymbolic(py::module_ &m)
{
    auto const addOp
        = [](ExprHandle const &a, ExprHandle const &b) { return a + b; };
    auto const subOp
        = [](ExprHandle const &a, ExprHandle const &b) { return a - b; };
    auto const mulOp
        = [](ExprHandle const &a, ExprHandle const &b) { return a * b; };
    auto const divOp
        = [](ExprHandle const &a, ExprHandle const &b) { return a / b; };
    auto const powOp = [](ExprHandle const &a, ExprHandle const &b) {
        return csl::api::power(a, b);
    };

    py::class_<ExprHandle> expr(
        m,
        "Expr",
        R"doc(Symbolic expression: a number, a symbol, or a compound expression.

Expressions are immutable: operators and methods return new expressions.
Numbers (int, fractions.Fraction, float, complex) mix with expressions in
operators. Equal expressions compare equal, whatever the order of terms:
x + y == y + x. Symbols and numbers are hashable; compound expressions are
not, use structural_key() as a key instead. complex(), float() and int()
convert an expression without free symbols.

>>> x = variable("x")
>>> print(2 * x + 3 * x)
5*x
>>> (x + 1) == (1 + x)
True
)doc");
    expr.def(
            py::init([](py::handle value) { return coerce(value); }),
            py::arg("value"),
            R"doc(Expression with the value of a number: int, fractions.Fraction, float or complex.

Raises TypeError for bool and str, and ValueError for any other object.

>>> Expr(2) == 2
True
>>> float(Expr(0.5))
0.5
>>> Expr(True)
Traceback (most recent call last):
...
TypeError: Expr does not accept bool or str values
)doc")
        .def("__add__",
             [=](py::handle lhs, py::handle rhs) {
                 return binary(lhs, rhs, addOp);
             })
        .def("__radd__",
             [=](py::handle self, py::handle other) {
                 return binary(other, self, addOp);
             })
        .def("__sub__",
             [=](py::handle lhs, py::handle rhs) {
                 return binary(lhs, rhs, subOp);
             })
        .def("__rsub__",
             [=](py::handle self, py::handle other) {
                 return binary(other, self, subOp);
             })
        .def("__mul__",
             [=](py::handle lhs, py::handle rhs) {
                 return binary(lhs, rhs, mulOp);
             })
        .def("__rmul__",
             [=](py::handle self, py::handle other) {
                 return binary(other, self, mulOp);
             })
        .def("__truediv__",
             [=](py::handle lhs, py::handle rhs) {
                 return binary(lhs, rhs, divOp);
             })
        .def("__rtruediv__",
             [=](py::handle self, py::handle other) {
                 return binary(other, self, divOp);
             })
        .def("__pow__",
             [=](py::handle lhs, py::handle rhs) {
                 return binary(lhs, rhs, powOp);
             })
        .def("__rpow__",
             [=](py::handle self, py::handle other) {
                 return binary(other, self, powOp);
             })
        .def("__neg__", [](ExprHandle const &self) { return -self; })
        .def("__eq__",
             [](py::handle lhs, py::handle rhs) -> py::object {
                 auto const left  = tryAsExpr(lhs);
                 auto const right = tryAsExpr(rhs);
                 if (!left or !right) {
                     return py::reinterpret_borrow<py::object>(
                         Py_NotImplemented);
                 }
                 return py::bool_(left->equals(*right));
             })
        .def("__hash__",
             [](ExprHandle const &self) -> py::ssize_t {
                 self.checkHashable();
                 if (self.isSymbol()) {
                     return py::hash(py::int_(static_cast<unsigned long long>(
                         self.symbolIdentity())));
                 }
                 return py::hash(numberValue(*self.number()));
             })
        .def(
            "structural_key",
            [](ExprHandle const &self) {
                return structuralKey(self.structure());
            },
            R"doc(Hashable key, equal for equal expressions whatever the order of their terms.
Use it as a dictionary key for compound expressions.

>>> x = variable("x")
>>> y = variable("y")
>>> (x + y).structural_key() == (y + x).structural_key()
True
)doc");
    bindTransforms(expr);
    expr.def(
        "__call__",
        [](ExprHandle const &self, py::handle operand) {
            return csl::api::applyTensorDerivative(self, coerce(operand));
        },
        py::arg("operand"),
        R"doc(Applies a tensor derivative to an expression. Only a derivative element made by a TensorDerivative can be called this way.

>>> sp = Space("S", 4)
>>> X = tensor("X", [sp])
>>> d = tensor_derivative("d", sp)
>>> i, j = sp.index("i"), sp.index("j")
>>> [index.name for index in d(i, X)(X(j)).free_indices]
['i', 'j']
)doc");
    expr.def_property_readonly(
            "free_indices",
            [](ExprHandle const &self) {
                return indexTuple(csl::api::freeIndices(self), py::none());
            },
            R"doc(Free indices of the expression, without duplicates. Their space is None.)doc")
        .def_property_readonly(
            "dummy_indices",
            [](ExprHandle const &self) {
                return indexTuple(csl::api::dummyIndices(self), py::none());
            },
            R"doc(Dummy indices of the expression, without duplicates. Their space is None.)doc")
        .def_property_readonly(
            "index_count",
            [](ExprHandle const &self) { return csl::api::indexCount(self); },
            R"doc(Number of indices of an indexed element, 0 for any other expression.

>>> (variable("x") * variable("y")).index_count
0
)doc")
        .def(
            "index",
            [](ExprHandle const &self, long long index) {
                if (index < 0) {
                    throw csl::api::LookupError("index: negative index");
                }
                return IndexWrapper{
                    csl::api::indexAt(self, static_cast<std::size_t>(index)),
                    py::none()};
            },
            py::arg("index"),
            R"doc(Index at the given position of an indexed element. MartyLookupError when out of range.

>>> euclid = Euclid_R3
>>> tensor("T", [euclid])(euclid.index("a")).index(1)
Traceback (most recent call last):
    ...
pymarty.errors.MartyLookupError: index: index 1 is out of range
)doc")
        .def_property_readonly(
            "index_structure",
            [](ExprHandle const &self) {
                return indexTuple(csl::api::indexStructure(self), py::none());
            },
            R"doc(Raw index structure of the expression, in CSL order, with repetitions. Their space is None.)doc")
        .def_property_readonly(
            "args",
            [](ExprHandle const &self) { return exprTuple(self.arguments()); },
            R"doc(Arguments of a compound expression, in order. Empty for a symbol or a number.

>>> x, y = variable("x"), variable("y")
>>> len((x + y).args)
2
>>> x.args
()
)doc")
        .def(
            "argument",
            [](ExprHandle const &self, long long index) {
                if (index < 0) {
                    throw csl::api::LookupError("argument: negative index");
                }
                return self.argument(static_cast<std::size_t>(index));
            },
            py::arg("index"),
            R"doc(Argument at the given position. MartyLookupError when out of range.

>>> x, y = variable("x"), variable("y")
>>> (x * y).argument(5)
Traceback (most recent call last):
    ...
pymarty.errors.MartyLookupError: argument: index 5 is out of range
)doc")
        .def_property_readonly(
            "operand",
            [](ExprHandle const &self) { return self.operand(); },
            R"doc(Operand of an operator, such as a derivative or a real part. MartyTypeError for other expressions.)doc")
        .def_property_readonly(
            "numerical_factor",
            [](ExprHandle const &self) { return self.numericalFactor(); },
            R"doc(Numerical factor in front of the expression, 1 if there is none.

>>> x = variable("x")
>>> (3 * x).numerical_factor
3
)doc")
        .def_property_readonly(
            "term",
            [](ExprHandle const &self) -> py::object {
                auto term = self.term();
                return term ? py::cast(*term) : py::none();
            },
            R"doc(Expression without its numerical factor, None if there is none.

>>> x, y = variable("x"), variable("y")
>>> (3 * x * y).term == x * y
True
)doc")
        .def_property_readonly(
            "factors",
            [](ExprHandle const &self) { return exprTuple(self.factors()); },
            R"doc(Factors of a product, each power as its own entry. The expression alone for other expressions.)doc")
        .def(
            "exponents",
            [](ExprHandle const &self, py::iterable factors) {
                std::vector<ExprHandle> list;
                for (py::handle factor : factors) {
                    list.push_back(coerce(factor));
                }
                return exprTuple(self.exponents(list));
            },
            py::arg("factors"),
            R"doc(Exponent of each given factor in the expression, 0 when the factor is absent.

>>> x, y, z = variable("x"), variable("y"), variable("z")
>>> [str(e) for e in (x**2 * y).exponents([x, y, z])]
['2', '1', '0']
)doc")
        .def_property_readonly(
            "numerator",
            [](ExprHandle const &self) { return self.numerator(); },
            R"doc(Numerator of an integer or rational. MartyTypeError for other expressions.

>>> variable("x").numerator
Traceback (most recent call last):
    ...
pymarty.errors.MartyTypeError: numerator: expression is not an integer or a rational
)doc")
        .def_property_readonly(
            "denominator",
            [](ExprHandle const &self) { return self.denominator(); },
            R"doc(Denominator of an integer (1) or rational. MartyTypeError for other expressions.

>>> variable("x").denominator
Traceback (most recent call last):
    ...
pymarty.errors.MartyTypeError: denominator: expression is not an integer or a rational
)doc")
        .def_property_readonly(
            "valued",
            [](ExprHandle const &self) { return self.valued(); },
            R"doc(True for a real number, or for a valued constant or variable. Compound expressions are not valued.

>>> variable("x").valued
False
)doc")
        .def_property_readonly(
            "value",
            [](ExprHandle const &self) {
                return static_cast<double>(self.value());
            },
            R"doc(Numerical value of a valued expression. MartyTypeError when the expression is not valued.

>>> variable("x").value
Traceback (most recent call last):
    ...
pymarty.errors.MartyTypeError: value: expression is not valued
)doc")
        .def_property_readonly(
            "type",
            [](ExprHandle const &self) { return self.type(); },
            R"doc(Type of the expression, as an ExprType.

>>> x, y = variable("x"), variable("y")
>>> (x + y).type == ExprType.Sum
True
)doc")
        .def_property_readonly(
            "primary_type",
            [](ExprHandle const &self) { return self.primaryType(); },
            R"doc(Primary type of the expression, as a PrimaryType.

>>> variable("x").primary_type == PrimaryType.Literal
True
)doc")
        .def_property_readonly(
            "dim",
            [](ExprHandle const &self) { return self.dim(); },
            R"doc(Dimension of the expression, 0 when it does not apply.)doc")
        .def_property_readonly(
            "shape",
            [](ExprHandle const &self) {
                std::vector<int> const shape = self.shape();
                py::tuple              result(shape.size());
                for (std::size_t i = 0; i < shape.size(); ++i) {
                    result[i] = shape[i];
                }
                return result;
            },
            R"doc(Shape of the expression as a tuple. A scalar has shape (1,).

>>> variable("x").shape
(1,)
)doc")
        .def_property_readonly(
            "latex_name",
            [](ExprHandle const &self) { return self.latexName(); },
            R"doc(LaTeX name of the expression, an empty string when it has none.

>>> variable("x").latex_name
'x'
>>> (variable("x") + variable("y")).latex_name
''
)doc")
        .def_property_readonly(
            "is_building_block",
            [](ExprHandle const &self) { return self.isBuildingBlock(); },
            R"doc(True for a building block (symbol, number or tensor element).

>>> variable("x").is_building_block
True
)doc")
        .def_property_readonly(
            "is_arbitrary",
            [](ExprHandle const &self) { return self.isArbitrary(); },
            R"doc(True for an arbitrary symbol.

>>> variable("x").is_arbitrary
False
)doc")
        .def_property_readonly(
            "is_indexed",
            [](ExprHandle const &self) { return self.isIndexed(); },
            R"doc(True for an expression carrying indices.

>>> variable("x").is_indexed
False
)doc")
        .def_property_readonly(
            "is_real",
            [](ExprHandle const &self) { return self.isReal(); },
            R"doc(True when the expression is real.

>>> Expr(1.5).is_real
True
)doc")
        .def_property_readonly(
            "is_purely_imaginary",
            [](ExprHandle const &self) { return self.isPurelyImaginary(); },
            R"doc(True for a purely imaginary expression.

>>> Expr(1.5).is_purely_imaginary
False
)doc")
        .def_property_readonly(
            "is_complex_conjugate",
            [](ExprHandle const &self) { return self.isComplexConjugate(); },
            R"doc(True for a complex conjugate operator.

>>> variable("z", complex_property=ComplexProperty.Complex).conjugate().is_complex_conjugate
True
)doc")
        .def_property_readonly(
            "is_hermitian_conjugate",
            [](ExprHandle const &self) { return self.isHermitianConjugate(); },
            R"doc(True for a hermitian conjugate operator.

>>> variable("x").is_hermitian_conjugate
False
)doc")
        .def_property_readonly(
            "is_integer",
            [](ExprHandle const &self) { return self.isInteger(); },
            R"doc(True for an integer number.

>>> Expr(3).is_integer
True
>>> Expr(1.5).is_integer
False
)doc")
        .def_property_readonly(
            "parent_id",
            [](ExprHandle const &self) -> py::object {
                auto const identity = self.parentIdentity();
                if (!identity) {
                    return py::none();
                }
                return py::int_(static_cast<unsigned long long>(*identity));
            },
            R"doc(Identity of the CSL parent (tensor, tensor field, variable, constant, ...), or None if there is none.
Equal for expressions sharing a parent, such as two elements of one tensor.

>>> x = variable("x")
>>> x.parent_id == x.parent_id
True
>>> (x + 1).parent_id is None
True
)doc")
        .def_property_readonly(
            "is_an_operator",
            [](ExprHandle const &self) { return self.isAnOperator(); },
            R"doc(True for an operator (derivative, conjugate, ...).

>>> variable("z", complex_property=ComplexProperty.Complex).real_part().is_an_operator
True
>>> variable("x").is_an_operator
False
)doc");
    expr.def(
        "relabel_indices",
        [](ExprHandle const &self, py::object renaming, bool relabelFree) {
            if (renaming.is_none()) {
                return csl::api::relabelIndices(self, relabelFree);
            }
            IndexRenamingHandle handle = renamingArgument(renaming);
            py::dict   dict = py::reinterpret_borrow<py::dict>(renaming);
            ExprHandle relabelled
                = csl::api::relabelIndices(self, relabelFree, handle);
            storeRenaming(dict, handle);
            return relabelled;
        },
        py::arg("renaming") = py::none(),
        py::kw_only(),
        py::arg("relabel_free") = true,
        R"doc(Renames the indices of the expression to canonical names. Returns a new
expression. Dummy indices are always renamed. With relabel_free=False, free
indices keep their names.

renaming is an optional dict from free Index to free Index, used as input and
output. A free index of the expression already in the dict keeps the new index
given there. The other free indices get canonical names, which are added to the
dict. Entries for other indices are kept. Dummy indices never enter the dict.

>>> sp = Space("S", 4)
>>> i, j = sp.index("i"), sp.index("j")
>>> A = tensor("A", [sp, sp])
>>> A(i, j).relabel_indices(relabel_free=False) == A(i, j)
True
>>> renaming = {}
>>> relabelled = A(i, j).relabel_indices(renaming)
>>> len(renaming)
2
)doc");

    m.attr("I")        = py::cast(ExprHandle::imaginaryUnit());
    m.attr("infinity") = py::cast(ExprHandle::infinity());
    m.attr("pi")       = py::cast(ExprHandle::pi());
    m.attr("e")        = py::cast(ExprHandle::e());

    py::enum_<csl::ComplexProperty>(m,
                                    "ComplexProperty",
                                    "Complex property of a symbol or a "
                                    "tensor: Complex, Real or Imaginary.")
        .value("Complex", csl::ComplexProperty::Complex)
        .value("Real", csl::ComplexProperty::Real)
        .value("Imaginary", csl::ComplexProperty::Imaginary);
    m.def(
        "make_abbreviation",
        [](std::string const &name, ExprHandle const &expr) {
            return csl::api::makeAbbreviation(name, expr);
        },
        py::arg("name"),
        py::arg("expr"),
        R"doc(Abbreviation named ``name`` standing for ``expr``.

The abbreviation is an expression of its own. ``evaluate()`` replaces it by
``expr``, and the abbreviations it contains are expanded too.

>>> a = variable("a")
>>> b = variable("b")
>>> ab = make_abbreviation("ab", a * b)
>>> (3 * ab).evaluate() == 3 * a * b
True
)doc");
    m.def(
        "constant",
        [](std::string const &name, csl::ComplexProperty complexProperty) {
            return ExprHandle::constant(name, complexProperty);
        },
        py::arg("name"),
        py::kw_only(),
        py::arg("complex_property") = csl::ComplexProperty::Real,
        R"doc(Creates a constant symbol. Each call gives a distinct symbol, even with the same name.

A constant does not depend on the variables: diff() treats it as a fixed value.
complex_property is ComplexProperty.Real by default.

>>> a = constant("a")
>>> a == constant("a")
False
)doc");
    m.def(
        "variable",
        [](std::string const &name, csl::ComplexProperty complexProperty) {
            return ExprHandle::variable(name, complexProperty);
        },
        py::arg("name"),
        py::kw_only(),
        py::arg("complex_property") = csl::ComplexProperty::Real,
        R"doc(Creates a variable symbol. Each call gives a distinct symbol, even with the same name.
complex_property is ComplexProperty.Real by default.

>>> x = variable("x")
>>> x == variable("x")
False
>>> x.name
'x'
)doc");

    defineFunction<csl::api::Sqrt>(
        m,
        "sqrt",
        R"doc(Square root. Exact for perfect squares, such as sqrt(4) == 2; other
arguments stay symbolic.

>>> sqrt(4) == 2
True
)doc");
    defineFunction<csl::api::Exp>(m,
                                  "exp",
                                  R"doc(Exponential.

>>> exp(0) == 1
True
)doc");
    defineFunction<csl::api::Log>(
        m,
        "log",
        R"doc(Natural logarithm. Other arguments stay symbolic.

>>> log(1) == 0
True
)doc");
    defineFunction<csl::api::Sin>(m, "sin", "Sine.");
    defineFunction<csl::api::Cos>(m, "cos", "Cosine.");
    defineFunction<csl::api::Tan>(m, "tan", "Tangent.");
    defineFunction<csl::api::Asin>(m, "asin", "Arc sine.");
    defineFunction<csl::api::Acos>(m, "acos", "Arc cosine.");
    defineFunction<csl::api::Atan>(m, "atan", "Arc tangent.");
    defineFunction<csl::api::Sinh>(m, "sinh", "Hyperbolic sine.");
    defineFunction<csl::api::Cosh>(m, "cosh", "Hyperbolic cosine.");
    defineFunction<csl::api::Tanh>(m, "tanh", "Hyperbolic tangent.");
    defineFunction<csl::api::Asinh>(m, "asinh", "Inverse hyperbolic sine.");
    defineFunction<csl::api::Acosh>(m, "acosh", "Inverse hyperbolic cosine.");
    defineFunction<csl::api::Atanh>(m, "atanh", "Inverse hyperbolic tangent.");
    defineFunction<csl::api::Abs>(
        m,
        "abs",
        R"doc(Absolute value. Other arguments, such as -x, stay symbolic.

>>> abs(-3) == 3
True
)doc");
    defineFunction<csl::api::modulus>(m,
                                      "modulus",
                                      R"doc(Modulus of a complex expression.

>>> modulus(3 + 4j) == 5
True
)doc");
    defineFunction<csl::api::argument>(
        m,
        "argument",
        R"doc(Argument (phase) of a complex expression.

>>> argument(-1) == pi
True
)doc");
    bindTensorFacade(m);
}
