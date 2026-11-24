"""Usage example: simulation (draft, target API of phase 6).

One script does everything: generate a MARTY library, compile it, import it,
and call its functions to fill the values of a plot.

This is the target API of phase 6 (see docs/architecture/pymarty/phase-6/03-spec.md).
It does not run until the phase 6 implementation is merged.

Run it with ``python simulation.py``. It is also run by the test suite
(marked ``needs_compiler`` and ``slow``).
"""

from pathlib import Path

from pymarty import simulation

WORK = Path("work")

# 1. Generate the library from a model. The returned Library object only
#    describes the library; nothing is compiled yet. The model is a placeholder
#    until the model class for simulations is chosen.
simple_spectrum_model = None  # placeholder: a pymarty.modeling.Model instance
lib = simulation.generate(
    model=simple_spectrum_model,  # a pymarty.modeling.Model instance (placeholder)
    path=WORK,
    name="toy",
)

# 2. Compile it. build() also adds the library directory to sys.path and
#    installs the importer, so the next import works without sys.path code.
lib.build()

# 3. Import the compiled library like a Python module. The generated toy.pyi
#    gives linters and type checkers the symbols.
from toy import Params, mass_A, mass_H  # noqa: E402  (import after build())

# 4. Call the generated functions with plain keyword arguments. Each function
#    only needs the parameters it uses; the others are not required.
masses = [mass_H(vph=246.0, mhH2=125.0), mass_A(vph=246.0, mhH2=125.0)]
print("masses:", masses)

# 4b. The same values through the params object generated for the library.
#     Fields not set stay None; a function checks only the fields it uses.
params = Params()
params.vph = 246.0
params.mhH2 = 125.0
print("masses from params:", mass_H(params), mass_A(params))

# 5. An undefined parameter is an error, not a silent zero (like InitSanitizer
#    in the C++ layer). mhH2 is missing here.
try:
    mass_H(vph=246.0)
except ValueError as error:
    print(f"caught: {error}")

# 5b. Partially filled params object. Suppose the library has three parameters
#     (vph, mhH2, and gS) and mass_H uses only vph and mhH2. The rule: ValueError
#     if and only if one of the two used parameters is None; gS may stay None.
partial = Params()
partial.gS = 1.2  # unused by mass_H: must not matter
for field in ("vph", "mhH2"):
    try:
        mass_H(partial)
    except ValueError:
        pass
    else:
        raise AssertionError(f"expected ValueError with {field} unset")
    setattr(partial, field, 246.0 if field == "vph" else 125.0)
assert mass_H(partial) == mass_H(vph=246.0, mhH2=125.0)
print("partial params: ValueError only while a used field is None")

# 6. Scan a parameter to produce plot data. The values are plain floats,
#    ready for matplotlib or any other post-processing.
xs = [100.0 + 10.0 * i for i in range(30)]
ys = [mass_H(vph=246.0, mhH2=x) for x in xs]
print(f"scanned {len(xs)} points, last value {ys[-1]:.6g}")

# 7. Optional: plot the scan. matplotlib is not a pymarty dependency.
try:
    import matplotlib.pyplot as plt
except ImportError:
    print("matplotlib not installed: skipping the plot")
else:
    plt.plot(xs, ys)
    plt.xlabel("mhH2")
    plt.ylabel("mass_H")
    plt.savefig(WORK / "scan.png")
    print("wrote", WORK / "scan.png")
