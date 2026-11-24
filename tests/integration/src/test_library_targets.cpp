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

struct Targets {
    bool cpp;
    bool python;
};

using FunctionBuilder = std::function<void(csl::LibraryGenerator &)>;

// Scoped switch of the global quadruple precision flag of the generator.
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
std::string runCDriver(fs::path const &dir,
                       std::string const &name,
                       std::string const &source)
{
    fs::path src = dir / "driver.c";
    fs::path bin = dir / "driver_c";
    std::ofstream(src) << source;
    runCommand(std::string(LT_STRINGIFY(MARTY_CC)) + " -std=c11 -I" +
               (dir / "clib").string() + " " + src.string() + " -o " +
               bin.string() + " -L" + (dir / "lib").string() + " -l" + name +
               " -Wl,-rpath," + (dir / "lib").string() + " -lquadmath");
    return runCommand(bin.string());
}

std::string runCppDriver(fs::path const &dir,
                         std::string const &name,
                         std::string const &source,
                         std::string const &defines = "")
{
    fs::path src = dir / "driver.cpp";
    fs::path bin = dir / "driver_cpp";
    std::ofstream(src) << source;
    runCommand(std::string(LT_STRINGIFY(MARTY_CXX)) + " -std=c++17 -I" +
               (dir / "include").string() + " -I" +
               LT_STRINGIFY(MARTY_INCLUDE_PATH) + " " + defines + " " +
               src.string() + " -o " + bin.string() + " -L" +
               (dir / "lib").string() + " -l" + name + " -Wl,-rpath," +
               (dir / "lib").string() + " -lquadmath");
    return runCommand(bin.string());
}

std::string runPythonDriver(fs::path const &dir, std::string const &source)
{
    fs::path src = dir / "driver.py";
    std::ofstream(src) << source;
    return runCommand("python3 " + src.string());
}

std::vector<long double> parseValues(std::string const &output)
{
    std::istringstream        in(output);
    std::vector<long double> values;
    long double               value;
    while (in >> value)
        values.push_back(value);
    return values;
}

void addBareFunctions(csl::LibraryGenerator &lib)
{
    csl::Expr x = csl::constant_s("x");
    csl::Expr y = csl::constant_s("y");
    lib.addFunction("f", x * y + 2 * x);
    lib.addFunction("g", CSL_I * x * y);
}

void addQuadFunctions(csl::LibraryGenerator &lib)
{
    lib.addFunction("h", 1 + csl::constant_s("x"));
}

// Evaluates f(2, 3) = 10 and g(2, 3) = 6i, printing real and imaginary parts.
void checkBareValues(fs::path const &dir, std::string const &name, Targets t)
{
    std::string const cSource = R"C(
#include <stdio.h>
#include <stdlib.h>
#include <quadmath.h>
#include "c_f.h"
#include "c_g.h"
static void show(__float128 v)
{
    char buf[128];
    quadmath_snprintf(buf, sizeof buf, "%.30Qg", v);
    printf("%s ", buf);
}
int main(void)
{
    creal_t x = 2.0, y = 3.0;
    ccomplex_return_t f = c_f_explicit(x, y);
    ccomplex_return_t g = c_g_explicit(x, y);
    show(f.real);
    show(f.imag);
    show(g.real);
    show(g.imag);
    printf("\n");
    return 0;
}
)C";

    std::string const cppSource = R"CPP(
#include <stdio.h>
#include <quadmath.h>
#include ")CPP" + name + R"CPP(.h"
static void show(__float128 v)
{
    char buf[128];
    quadmath_snprintf(buf, sizeof buf, "%.30Qg", v);
    printf("%s ", buf);
}
int main()
{
    )CPP" + name + R"CPP(::param_t p;
    p.x = 2.0;
    p.y = 3.0;
    auto f = )CPP" + name + R"CPP(::f(p);
    auto g = )CPP" + name + R"CPP(::g(p);
    show(f.real());
    show(f.imag());
    show(g.real());
    show(g.imag());
    printf("\n");
    return 0;
}
)CPP";

    std::string const pythonSource = "import sys\n"
                                     "sys.path.insert(0, '" +
                                     (dir / "py").string() +
                                     "')\n"
                                     "import " +
                                     name +
                                     " as m\n"
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

void checkBareLibrary(std::string const &name, Targets targets)
{
    fs::path dir = generate(name, targets, false, addBareFunctions);
    checkBareValues(dir, name, targets);
}

} // namespace

TEST(library_targets, bare_c_only)
{
    checkBareLibrary("itgc", {false, false});
}

TEST(library_targets, bare_cpp)
{
    checkBareLibrary("itgcpp", {true, false});
}

TEST(library_targets, bare_python)
{
    checkBareLibrary("itgpy", {false, true});
}

TEST(library_targets, bare_cpp_python)
{
    checkBareLibrary("itgcpypy", {true, true});
}

// With quadruple precision, 1 + 1e-20 keeps the small term, double does not.
TEST(library_targets, quadruple_c_only)
{
    fs::path dir = generate("itgqc", {false, false}, true, addQuadFunctions);
    std::string const source = R"C(
#include <stdio.h>
#include <stdlib.h>
#include <quadmath.h>
#include "c_h.h"
int main(void)
{
    creal_t x = strtoflt128("1e-20", NULL);
    char buf[128];
    ccomplex_return_t h = c_h_explicit(x);
    quadmath_snprintf(buf, sizeof buf, "%.30Qg", (__float128)(h.real - 1));
    printf("%s\n", buf);
    return 0;
}
)C";
    std::vector<long double> values =
        parseValues(runCDriver(dir, "itgqc", source));
    ASSERT_EQ(values.size(), 1u);
    EXPECT_NEAR(values[0], 1e-20L, 1e-30L);
}

TEST(library_targets, quadruple_cpp)
{
    fs::path dir = generate("itgqcpp", {true, false}, true, addQuadFunctions);
    std::string const source = R"CPP(
#include <stdio.h>
#include <stdlib.h>
#include <quadmath.h>
#include "itgqcpp.h"
int main()
{
    itgqcpp::param_t p;
    p.x = strtoflt128("1e-20", NULL);
    auto h = itgqcpp::h(p);
    char buf[128];
    quadmath_snprintf(buf, sizeof buf, "%.30Qg", (__float128)(crealq(h) - 1));
    printf("%s\n", buf);
    return 0;
}
)CPP";
    std::vector<long double> values =
        parseValues(runCppDriver(dir, "itgqcpp", source, "-DQUAD=1 -DQUADSIZE=16"));
    ASSERT_EQ(values.size(), 1u);
    EXPECT_NEAR(values[0], 1e-20L, 1e-30L);
}

TEST(library_targets, python_rejects_quadruple_precision)
{
    QuadScope scope(true);
    fs::path  root = fs::absolute(fs::path("libs") / "integration");
    csl::LibraryGenerator lib("itgqpy", root.string());
    lib.setCppLibrary(false);
    lib.setPythonLibrary(true);
    addQuadFunctions(lib);
    EXPECT_ANY_THROW(lib.print());
}
