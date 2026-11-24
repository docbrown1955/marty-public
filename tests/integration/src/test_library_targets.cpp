#include "csl/librarygenerator.h"
#include "marty.h"
#include <gtest/gtest.h>

#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <functional>
#include <sstream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace {

#define LT_STRINGIFY2(X) #X
#define LT_STRINGIFY(X) LT_STRINGIFY2(X)

// Combinations of optional layers on top of the C library.
struct Targets {
    std::string label;
    bool        cpp;
    bool        python;
};

std::vector<Targets> const allTargets = {
    {"c", false, false},
    {"cpp", true, false},
    {"python", false, true},
    {"cpp_python", true, true},
};

std::string targetName(::testing::TestParamInfo<Targets> const &info)
{
    return info.param.label;
}

using FunctionBuilder = std::function<void(csl::LibraryGenerator &)>;

// Scoped switch of the quadruple precision option of the C implementation.
class QuadScope {
  public:
    explicit QuadScope(bool quad)
    {
        csl::LibraryGenerator::setQuadruplePrecision(quad);
    }
    ~QuadScope()
    {
        csl::LibraryGenerator::setQuadruplePrecision(false);
    }
};

std::string runCommand(std::string const &cmd)
{
    std::string output;
    FILE       *pipe = popen((cmd + " 2>&1").c_str(), "r");
    if (pipe == nullptr) {
        ADD_FAILURE() << "Cannot run " << cmd;
        return output;
    }
    char buffer[512];
    while (fgets(buffer, sizeof buffer, pipe) != nullptr)
        output += buffer;
    int status = pclose(pipe);
    EXPECT_EQ(status, 0) << cmd << "\n" << output;
    return output;
}

fs::path generate(std::string const &name,
                  Targets            targets,
                  bool               quad,
                  FunctionBuilder    addFunctions)
{
    QuadScope scope(quad);
    fs::path  root = fs::absolute(fs::path("libs") / "integration");
    fs::path  dir  = root / name;
    fs::remove_all(dir);

    csl::LibraryGenerator lib(name, root.string());
    lib.setCppLibrary(targets.cpp);
    lib.setPythonLibrary(targets.python);
    lib.addIPath(LT_STRINGIFY(MARTY_INCLUDE_PATH));
    addFunctions(lib);
    lib.print();
    runCommand("make -C " + dir.string() + " lib");
    return dir;
}

// Driver programs are compiled with the same compilers as the generator.
// Extra flags are appended to the link line (used for LoopTools).
std::string runCDriver(fs::path const    &dir,
                       std::string const &name,
                       std::string const &source,
                       std::string const &extraFlags = "")
{
    fs::path src = dir / "driver.c";
    fs::path bin = dir / "driver_c";
    std::ofstream(src) << source;
    runCommand(std::string(LT_STRINGIFY(MARTY_CC)) + " -std=c11 -I" +
               (dir / "clib").string() + " " + src.string() + " -o " +
               bin.string() + " -L" + (dir / "lib").string() + " -l" + name +
               " -Wl,-rpath," + (dir / "lib").string() + " " + extraFlags);
    return runCommand(bin.string());
}

// Libraries using MARTY's LoopTools link against the build tree copy.
std::string loopToolsFlags()
{
    std::string root = std::string(MARTY_LIBRARY_PATH) + "/..";
    return "-L" + root + " -looptools -lgfortran -Wl,-rpath," + root;
}

std::string runCppDriver(fs::path const    &dir,
                         std::string const &name,
                         std::string const &source,
                         std::string const &extraFlags = "")
{
    fs::path src = dir / "driver.cpp";
    fs::path bin = dir / "driver_cpp";
    std::ofstream(src) << source;
    runCommand(std::string(LT_STRINGIFY(MARTY_CXX)) + " -std=c++17 -I" +
               (dir / "include").string() + " -I" +
               LT_STRINGIFY(MARTY_INCLUDE_PATH) + " " + src.string() +
               " -o " + bin.string() + " -L" + (dir / "lib").string() + " -l" +
               name + " -Wl,-rpath," + (dir / "lib").string() + " " +
               extraFlags);
    return runCommand(bin.string());
}

std::string runPythonDriver(fs::path const &dir, std::string const &source)
{
    fs::path src = dir / "driver.py";
    std::ofstream(src) << source;
    return runCommand("python3 " + src.string());
}

// Python driver preamble: the generated package is loaded from its directory.
std::string pythonImport(fs::path const &dir, std::string const &name)
{
    return "import sys\n"
           "sys.path.insert(0, '" +
           (dir / "py").string() +
           "')\n"
           "import " +
           name + " as m\n";
}

std::vector<long double> parseValues(std::string const &output)
{
    std::istringstream       in(output);
    std::vector<long double> values;
    long double              value;
    while (in >> value)
        values.push_back(value);
    return values;
}

// Public headers are C or C++ interfaces in double precision: no quadruple type or macro.
void checkPublicHeadersHaveNoQuad(fs::path const &dir)
{
    for (char const *sub : {"clib", "include"}) {
        fs::path root = dir / sub;
        if (!fs::exists(root))
            continue;
        for (auto const &entry : fs::recursive_directory_iterator(root)) {
            if (entry.path().extension() != ".h")
                continue;
            std::ifstream in(entry.path());
            std::stringstream text;
            text << in.rdbuf();
            for (std::string const &word :
                 {"quadmath", "QUAD", "float128", "complex128", "MTY_"})
                EXPECT_EQ(text.str().find(word), std::string::npos)
                    << entry.path() << " contains " << word;
        }
    }
}

void addBareFunctions(csl::LibraryGenerator &lib)
{
    csl::Expr x = csl::constant_s("x");
    csl::Expr y = csl::constant_s("y");
    lib.addFunction("f", x * y + 2 * x);
    lib.addFunction("g", CSL_I * x * y);
}

// exp(x) - 1 is not simplified by CSL. For x = 1e-20 it is 1e-20 with quadruple
// precision internally, and 0 in double precision.
void addQuadFunctions(csl::LibraryGenerator &lib)
{
    lib.addFunction("h", csl::exp_s(csl::constant_s("x")) - 1);
}

void addTrivialFunctions(csl::LibraryGenerator &lib)
{
    lib.addFunction("k", 3 * CSL_I);
}

// Evaluates f(2, 3) = 10 and g(2, 3) = 6i, printing real and imaginary parts.
void checkBareValues(fs::path const &dir, std::string const &name, Targets t)
{
    std::string const cSource = R"C(
#include <stdio.h>
#include "c_f.h"
#include "c_g.h"
int main(void)
{
    creal_t x = 2.0, y = 3.0;
    ccomplex_return_t f = c_f_explicit(x, y);
    ccomplex_return_t g = c_g_explicit(x, y);
    printf("%.17g %.17g %.17g %.17g\n", f.real, f.imag, g.real, g.imag);
    return 0;
}
)C";

    std::string const cppSource = R"CPP(
#include <stdio.h>
#include ")CPP" + name + R"CPP(.h"
int main()
{
    )CPP" + name + R"CPP(::param_t p;
    p.x = 2.0;
    p.y = 3.0;
    auto f = )CPP" + name + R"CPP(::f(p);
    auto g = )CPP" + name + R"CPP(::g(p);
    printf("%.17g %.17g %.17g %.17g\n", f.real(), f.imag(), g.real(), g.imag());
    return 0;
}
)CPP";

    std::string const pythonSource = pythonImport(dir, name) +
                                     "g = m.g(x=2.0, y=3.0)\n"
                                     "f = m.f(x=2.0, y=3.0)\n"
                                     "print(f.real, f.imag, g.real, g.imag)\n";

    std::vector<std::string> outputs;
    outputs.push_back(runCDriver(dir, name, cSource));
    if (t.cpp)
        outputs.push_back(runCppDriver(dir, name, cppSource));
    if (t.python)
        outputs.push_back(runPythonDriver(dir, pythonSource));

    for (std::string const &output : outputs) {
        std::vector<long double> values = parseValues(output);
        ASSERT_EQ(values.size(), 4u) << output;
        EXPECT_NEAR(values[0], 10.0L, 1e-12L);
        EXPECT_NEAR(values[1], 0.0L, 1e-12L);
        EXPECT_NEAR(values[2], 0.0L, 1e-12L);
        EXPECT_NEAR(values[3], 6.0L, 1e-12L);
    }
}

class BareLibrary : public ::testing::TestWithParam<Targets> {};

TEST_P(BareLibrary, evaluates_expressions)
{
    Targets     t   = GetParam();
    std::string name = "bare_" + t.label;
    fs::path    dir = generate(name, t, false, addBareFunctions);
    checkBareValues(dir, name, t);
}

INSTANTIATE_TEST_SUITE_P(library_targets,
                         BareLibrary,
                         ::testing::ValuesIn(allTargets),
                         targetName);

// A function without parameters that is a plain number is still exposed in C and Python.
TEST(library_targets, trivial_function)
{
    fs::path dir = generate("trivial", {"c_python", false, true}, false,
                            addTrivialFunctions);
    std::string const source = R"C(
#include <stdio.h>
#include "c_k.h"
int main(void)
{
    ccomplex_return_t k = c_k_explicit();
    printf("%g %g\n", k.real, k.imag);
    return 0;
}
)C";
    std::string const pythonSource = pythonImport(dir, "trivial") +
                                     "k = m.k()\n"
                                     "print(k.real, k.imag)\n";
    for (std::string const &output : {runCDriver(dir, "trivial", source),
                                      runPythonDriver(dir, pythonSource)}) {
        std::vector<long double> values = parseValues(output);
        ASSERT_EQ(values.size(), 2u) << output;
        EXPECT_NEAR(values[0], 0.0L, 1e-12L);
        EXPECT_NEAR(values[1], 3.0L, 1e-12L);
    }
}

// The quadruple option is internal to the C calculations: every public interface
// stays double and returns the quadruple result rounded to double.
class QuadLibrary : public ::testing::TestWithParam<Targets> {};

TEST_P(QuadLibrary, computes_internally_in_quadruple)
{
    Targets     t    = GetParam();
    std::string name = "prec_" + t.label;
    fs::path    dir  = generate(name, t, true, addQuadFunctions);
    checkPublicHeadersHaveNoQuad(dir);

    std::string const cSource = R"C(
#include <stdio.h>
#include "c_h.h"
int main(void)
{
    creal_t x = 1e-20;
    ccomplex_return_t h = c_h_explicit(x);
    printf("%.17g\n", h.real);
    return 0;
}
)C";
    std::string const cppSource = R"CPP(
#include <stdio.h>
#include ")CPP" + name + R"CPP(.h"
int main()
{
    )CPP" + name + R"CPP(::param_t p;
    p.x = 1e-20;
    printf("%.17g\n", )CPP" + name + R"CPP(::h(p).real());
    return 0;
}
)CPP";
    std::string const pythonSource = pythonImport(dir, name) +
                                     "print(m.h(x=1e-20).real)\n";

    std::vector<std::string> outputs;
    outputs.push_back(runCDriver(dir, name, cSource));
    if (t.cpp)
        outputs.push_back(runCppDriver(dir, name, cppSource));
    if (t.python)
        outputs.push_back(runPythonDriver(dir, pythonSource));

    for (std::string const &output : outputs) {
        std::vector<long double> values = parseValues(output);
        ASSERT_EQ(values.size(), 1u) << output;
        EXPECT_NEAR(values[0], 1e-20L, 1e-30L);
    }
}

INSTANTIATE_TEST_SUITE_P(library_targets,
                         QuadLibrary,
                         ::testing::ValuesIn(allTargets),
                         targetName);

// Shows that the check above discriminates: the same expression in double gives 0.
TEST(library_targets, double_loses_the_small_term)
{
    fs::path dir = generate("double_c", {"c", false, false}, false,
                            addQuadFunctions);
    std::string const source = R"C(
#include <stdio.h>
#include "c_h.h"
int main(void)
{
    creal_t x = 1e-20;
    ccomplex_return_t h = c_h_explicit(x);
    printf("%.17g\n", h.real);
    return 0;
}
)C";
    std::vector<long double> values =
        parseValues(runCDriver(dir, "double_c", source));
    ASSERT_EQ(values.size(), 1u);
    EXPECT_EQ(values[0], 0.0L);
}

// Generates a library with a single complex scalar of mass m under U(1).
fs::path generateSpectrumLibrary(std::string const &name, Targets targets)
{
    fs::path root = fs::absolute(fs::path("libs") / "integration");
    fs::path dir  = root / name;
    fs::remove_all(dir);

    csl::Expr e = csl::constant_s("e");
    mty::Model model({mty::GaugeGroupSpec{mty::group::Type::U, 1, "em", "A", e}});
    mty::Particle phi = mty::scalarboson_s("h", model);
    phi->setGroupRep("em", -1);
    model.addParticle(phi);
    csl::Expr m2 = csl::constant_s("m2");
    model.addLagrangianTerm(-m2 * csl::GetComplexConjugate(phi()) * phi());
    model.refresh();

    mty::Library lib(name, root.string());
    lib.setCppLibrary(targets.cpp);
    lib.setPythonLibrary(targets.python);
    lib.addIPath(LT_STRINGIFY(MARTY_INCLUDE_PATH));
    lib.addLPath(LT_STRINGIFY(MARTY_LIBRARY_PATH));
    lib.addLPath(LT_STRINGIFY(MARTY_LIBRARY_PATH) "/..");
    lib.addLPath(LT_STRINGIFY(MARTY_LIBRARY_PATH) "/marty");
    lib.generateSpectrum(model);
    lib.print();
    runCommand("make -C " + dir.string() + " lib");
    // The library depends on libooptools.so, which the drivers' RUNPATH does not reach.
    setenv("LD_LIBRARY_PATH", (std::string(MARTY_LIBRARY_PATH) + "/..").c_str(), 1);
    return dir;
}

// The mass term m2 |h|^2 gives m_h = sqrt(m2), so m2 = 4 must give m_h = 2.
// The spectrum update is exposed by the C and C++ layers only.
class SpectrumLibrary : public ::testing::TestWithParam<Targets> {};

TEST_P(SpectrumLibrary, updates_masses)
{
    Targets     t    = GetParam();
    std::string name = "spectrum_" + t.label;
    fs::path    dir  = generateSpectrumLibrary(name, t);
    std::string const cSource = R"C(
#include <stdio.h>
#include "cspectrum.h"
int main(void)
{
    cparam_t p;
    p.m2 = 4.0;
    c_updateSpectrum(&p);
    printf("VALUE %.17g\n", p.m_h);
    return 0;
}
)C";
    std::string const cppSource = R"CPP(
#include <stdio.h>
#include ")CPP" + name + R"CPP(.h"
int main()
{
    )CPP" + name + R"CPP(::param_t p;
    p.m2 = 4.0;
    )CPP" + name + R"CPP(::updateSpectrum(p);
    printf("VALUE %.17g\n", static_cast<double>(p.m_h));
    return 0;
}
)CPP";

    std::vector<std::pair<std::string, std::string>> outputs;
    outputs.emplace_back("C", runCDriver(dir, name, cSource, loopToolsFlags()));
    if (t.cpp)
        outputs.emplace_back("C++",
                             runCppDriver(dir, name, cppSource, loopToolsFlags()));

    for (auto const &[layer, output] : outputs) {
        // LoopTools writes a banner to stdout, so only the tagged line is parsed.
        std::string const tag = "VALUE ";
        std::size_t const pos = output.find(tag);
        ASSERT_NE(pos, std::string::npos) << layer << output;
        std::vector<long double> values =
            parseValues(output.substr(pos + tag.size()));
        ASSERT_EQ(values.size(), 1u) << layer << output;
        EXPECT_NEAR(values[0], 2.0L, 1e-12L) << layer;
    }

    // The Python module must load on top of the same compiled library.
    if (t.python)
        runPythonDriver(dir, pythonImport(dir, name) + "print(0)\n");
}

INSTANTIATE_TEST_SUITE_P(library_targets,
                         SpectrumLibrary,
                         ::testing::ValuesIn(allTargets),
                         targetName);

} // namespace
