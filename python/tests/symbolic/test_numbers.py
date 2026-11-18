"""Numbers and elementary functions of the symbolic API."""

import math
from fractions import Fraction

import pytest

from pymarty import symbolic
from pymarty.symbolic import Expr


@pytest.mark.unit
def test_integer_constants() -> None:
    assert Expr(0) == 0
    assert Expr(1) == 1
    assert Expr(2) == 2
    assert Expr(-2) == -2
    assert Expr(2) * Expr(-2) == -4
    assert Expr(-2) + 2 == 0


@pytest.mark.unit
def test_integer_complex_functions() -> None:
    assert symbolic.modulus(Expr(-2)) == 2
    assert symbolic.modulus(Expr(-3)) == 3
    assert symbolic.abs(Expr(-2)) == 2
    assert Expr(3) + 4 * symbolic.I == Expr(3) + Expr(4) * symbolic.I
    assert (Expr(3) + 4 * symbolic.I).conjugate() == Expr(3) - 4 * symbolic.I


@pytest.mark.unit
def test_pi_and_e_values() -> None:
    assert float(symbolic.pi) == pytest.approx(math.pi)
    assert float(symbolic.e) == pytest.approx(math.e)
    assert symbolic.pi.to_latex() == "\\pi"
    assert symbolic.e.to_latex() == "e"


@pytest.mark.unit
def test_float_complex_functions() -> None:
    z = Expr(3.0 + 4.0j)
    assert float(symbolic.modulus(z)) == pytest.approx(5.0)
    assert float(symbolic.argument(z)) == pytest.approx(math.atan2(4.0, 3.0))
    assert z.conjugate() == Expr(3.0 - 4.0j)


@pytest.mark.unit
def test_rationals_normalise_and_sign() -> None:
    assert Fraction(-2, 4) == Fraction(-1, 2)
    assert Expr(Fraction(-1, 3)) + Expr(Fraction(1, 3)) == 0
    assert Expr(Fraction(-1, 2)) * Expr(Fraction(-2, 3)) == Fraction(1, 3)
    assert Expr(Fraction(-3, 6)) == Expr(Fraction(-1, 2))


@pytest.mark.unit
def test_rational_constants() -> None:
    half = Expr(Fraction(1, 2))
    third = Expr(Fraction(1, 3))
    assert -half == Expr(Fraction(-1, 2))
    assert -third == Expr(Fraction(-1, 3))
    assert half * 2 == 1
    assert third * 3 == 1
    assert half * third == Expr(Fraction(1, 6))


@pytest.mark.unit
def test_rational_complex_functions() -> None:
    z = Expr(Fraction(1, 2)) * symbolic.I
    assert symbolic.modulus(z) == Fraction(1, 2)
    assert z.conjugate() == -z
    assert Expr(Fraction(1, 2)).conjugate() == Fraction(1, 2)


@pytest.mark.unit
def test_complex_numbers_full() -> None:
    assert Expr(1 + 2j) * Expr(3 - 1j) == Expr(5 + 5j)
    assert Expr(1 + 2j) + Expr(1 - 2j) == 2
    assert Expr(1 + 2j) != Expr(1 + 2j) * 2


@pytest.mark.unit
def test_power_with_rational_float_complex_exponents() -> None:
    assert Expr(4) ** Expr(Fraction(1, 2)) == 2
    assert float(Expr(2) ** 0.5) == pytest.approx(math.sqrt(2))
    assert float(Expr(0.5) ** -3) == pytest.approx(8.0)
    assert complex(Expr(1 + 2j) ** 4) == pytest.approx((1 + 2j) ** 4)


@pytest.mark.unit
def test_mixed_type_arithmetic() -> None:
    assert Expr(5) + Expr(Fraction(2, 3)) == Fraction(17, 3)
    assert Expr(5) * 0.5 == 2.5
    assert Expr(3) / 2 == Fraction(3, 2)
    assert Expr(2) + (1 + 1j) == Expr(3 + 1j)


@pytest.mark.unit
def test_complex_arithmetic_with_mixed_types() -> None:
    z = Expr(1 + 2j)
    assert z + 1 == Expr(2 + 2j)
    assert z * 0.5 == Expr(0.5 + 1j)
    assert z * Fraction(1, 2) == Expr(0.5 + 1j)


@pytest.mark.unit
def test_derivative_of_numbers_is_zero() -> None:
    a = symbolic.variable("a")
    assert Expr(2).diff(a) == 0
    assert Expr(2.5).diff(a) == 0
    assert Expr(Fraction(2, 5)).diff(a) == 0
    assert Expr(1 + 2j).diff(a) == 0


@pytest.mark.unit
@pytest.mark.parity("SymbolicApi.IntegerArithmeticExact")
def test_integer_arithmetic_is_exact() -> None:
    assert Expr(2) + Expr(3) == 5
    assert Expr(2) * 3 == 6
    assert 2 - Expr(3) == -1


@pytest.mark.unit
@pytest.mark.parity("SymbolicApi.RationalNormalisation")
def test_rationals_are_normalised() -> None:
    assert Expr(Fraction(2, 4)) == Expr(Fraction(1, 2))
    assert Expr(Fraction(4, 2)) == 2
    assert Expr(Fraction(1, 3)) + Expr(Fraction(2, 3)) == 1


@pytest.mark.unit
@pytest.mark.parity("SymbolicApi.FloatContagion")
def test_float_is_contagious() -> None:
    assert (Expr(1) + Expr(0.5)).structural_key() == ("number", 1.5)
    assert Expr(2.0) == Expr(2)


@pytest.mark.unit
@pytest.mark.parity("SymbolicApi.ComplexNumbers")
def test_complex_numbers() -> None:
    assert symbolic.I**2 == -1
    assert Expr(3 + 0j) == 3
    assert Expr(1 + 2j) == Expr(1 + 2j)
    assert Expr(1 + 2j) != Expr(1 - 2j)


@pytest.mark.unit
@pytest.mark.parity("SymbolicApi.FunctionsSpecialValues")
def test_functions_at_special_values() -> None:
    assert symbolic.sin(0) == 0
    assert symbolic.cos(0) == 1
    assert symbolic.exp(0) == 1
    assert symbolic.log(1) == 0
    assert symbolic.sqrt(4) == 2


@pytest.mark.unit
@pytest.mark.parity("SymbolicApi.PiAndEConstants")
def test_pi_and_e_are_stable_distinct_constants() -> None:
    assert symbolic.pi == symbolic.pi
    assert symbolic.e == symbolic.e
    assert symbolic.pi != symbolic.e
    assert float(symbolic.pi) == pytest.approx(math.pi, abs=1e-12)
    assert float(symbolic.e) == pytest.approx(math.e, abs=1e-12)


@pytest.mark.unit
@pytest.mark.parity("SymbolicApi.ModulusOfComplex")
def test_modulus_of_integer_complex() -> None:
    z = Expr(3) + Expr(4) * symbolic.I
    assert float(symbolic.modulus(z)) == pytest.approx(5.0, abs=1e-12)


@pytest.mark.unit
@pytest.mark.parity("SymbolicApi.ModulusOfRealIsAbsoluteValue")
def test_modulus_of_real_is_absolute_value() -> None:
    assert symbolic.modulus(Expr(-2)) == 2
    assert symbolic.modulus(Expr(3)) == 3
    assert symbolic.modulus(Expr(Fraction(-1, 2))) == Fraction(1, 2)
    assert symbolic.modulus(Expr(0)) == 0


@pytest.mark.unit
@pytest.mark.parity("SymbolicApi.ArgumentOfComplex")
def test_argument_of_complex_and_negative_real() -> None:
    z = Expr(3) + Expr(4) * symbolic.I
    assert float(symbolic.argument(z)) == pytest.approx(math.atan2(4.0, 3.0), abs=1e-12)
    assert float(symbolic.argument(Expr(-2))) == pytest.approx(math.pi, abs=1e-12)


@pytest.mark.unit
@pytest.mark.parity("SymbolicApi.AbsoluteValue")
def test_abs_of_integers_and_rationals() -> None:
    assert symbolic.abs(Expr(-3)) == 3
    assert symbolic.abs(Expr(3)) == 3
    assert symbolic.abs(Expr(Fraction(-1, 2))) == Fraction(1, 2)


@pytest.mark.unit
@pytest.mark.parametrize(
    "value",
    [True, False, "1"],
)
def test_expr_rejects_bool_and_str(value: object) -> None:
    with pytest.raises(TypeError):
        Expr(value)


@pytest.mark.unit
def test_expr_rejects_unconvertible_value() -> None:
    with pytest.raises(ValueError):
        Expr(object())
