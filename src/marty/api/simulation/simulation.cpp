/**
 * @file simulation.cpp
 * @brief Generation and compilation of a standalone MARTY library.
 */

#include "simulation.h"

#include "../../../csl/scopedProperty.h"
#include "../../core/ghostField.h"
#include "../../core/goldstoneField.h"
#include "../../core/mtylibrary.h"
#include "../../lha/lha.h"
#include "../common/error.h"
#include "../common/session.h"

#include <algorithm>
#include <cctype>
#include <fcntl.h>
#include <fstream>
#include <spawn.h>
#include <sys/wait.h>
#include <unistd.h>
#include <utility>

extern char **environ;

namespace mty::api::simulation {

namespace {

bool isCIdentifier(std::string const &name)
{
    if (name.empty() or std::isdigit(static_cast<unsigned char>(name.front())))
        return false;
    return std::all_of(name.begin(), name.end(), [](unsigned char c) {
        return std::isalnum(c) or c == '_';
    });
}

void checkName(std::string const &name, char const *what)
{
    if (not isCIdentifier(name))
        throw ValueError(std::string(what) + " '" + name
                         + "' is not a valid C identifier.");
}

// Runs `args` with stdout and stderr in `logPath`, and returns its exit code.
int runProcess(std::vector<std::string> const &args,
               std::string const              &logPath)
{
    std::vector<char *> argv;
    for (auto const &arg : args)
        argv.push_back(const_cast<char *>(arg.c_str()));
    argv.push_back(nullptr);

    posix_spawn_file_actions_t actions;
    posix_spawn_file_actions_init(&actions);
    posix_spawn_file_actions_addopen(&actions,
                                     STDOUT_FILENO,
                                     logPath.c_str(),
                                     O_WRONLY | O_CREAT | O_TRUNC,
                                     0644);
    posix_spawn_file_actions_adddup2(&actions, STDOUT_FILENO, STDERR_FILENO);

    pid_t pid = 0;
    int   rc
        = posix_spawnp(&pid, argv[0], &actions, nullptr, argv.data(), environ);
    posix_spawn_file_actions_destroy(&actions);
    if (rc != 0)
        throw StateError("Cannot start '" + args[0] + "'.");

    int status = 0;
    waitpid(pid, &status, 0);
    return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
}

} // namespace

Generator::Generator(std::string const &name, std::string const &directory)
    : m_generator(name, directory),
      m_name(name),
      m_directory(directory + "/" + name)
{
    checkName(name, "Library name");
    // The Python side loads the C core through its ctypes binding only.
    m_generator.setCppLibrary(false);
    m_generator.setPythonLibrary(true);
}

std::string Generator::libraryDirectory() const
{
    return m_directory;
}

void Generator::addIncludePath(std::string const &path)
{
    m_generator.addIPath(path);
}

void Generator::addLibraryPath(std::string const &path)
{
    m_generator.addLPath(path);
}

void Generator::addFunction(std::string const          &name,
                            csl::api::ExprHandle const &expr)
{
    checkName(name, "Function name");
    addFunctionNamed(name, expr.raw(), false);
}

void Generator::addFunctionNamed(std::string const &name,
                                 csl::Expr const   &expr,
                                 bool               clearCache)
{
    if (std::find(m_functions.begin(), m_functions.end(), name)
        != m_functions.end())
        throw ValueError("Function '" + name + "' is already defined.");
    m_functions.push_back(name);

    csl::ScopedProperty p(&csl::option::checkCommutations, false);
    auto               &f = m_generator.addFunction(name, expr);
    if (clearCache)
        f.addInitInstruction("clearcache();");
}

void Generator::addSpectrum(modeling::Model &model)
{
    if (m_hasSpectrum)
        throw StateError("The spectrum of library '" + m_name
                         + "' is already added.");
    guarded([&] {
        auto   &core = modeling::coreModel(model);
        Session session(&core);
        // Same steps as mty::Library::generateSpectrum, on the C generator.
        // The LoopTools include links LoopTools, needed by clearcache().
        m_generator.addInclude("clooptools.h");
        m_generator.addInclude("marty/core/looptools_init.h");
        core.applyDiagonalizationData(m_generator);
        for (auto const &m : core.getAbbreviatedMassExpressions()) {
            if (csl::Abbrev::find_opt(m)) {
                m_generator.addMassExpression(m->getName());
                addFunctionNamed(m->getName(), m, true);
            }
        }
        for (auto const &particle : core.getParticles()) {
            if (mty::IsOfType<mty::GhostBoson>(particle)
                or mty::IsOfType<mty::GoldstoneBoson>(particle))
                continue;
            for (csl::Expr const &value :
                 {particle->getMass(), particle->getWidth()}) {
                if (csl::IsLiteral(value))
                    m_generator.addDefaultParameter(
                        value->getName(),
                        value->getComplexProperty()
                            == csl::ComplexProperty::Complex);
            }
        }
    });
    m_hasSpectrum = true;
}

void Generator::computeModelWidths(modeling::Model &model,
                                   modeling::Order  order)
{
    guarded([&] {
        auto   &core = modeling::coreModel(model);
        Session session(&core);
        core.computeModelWidths(order == modeling::Order::TreeLevel
                                    ? mty::TreeLevel
                                    : mty::OneLoop);
    });
}

void Generator::setQuadruplePrecision(bool enabled)
{
    guarded([&] { m_generator.setQuadruplePrecision(enabled); });
}

void Generator::print()
{
    m_generator.print();
}

void Generator::build(unsigned int nJobs, std::string const &make)
{
    print();
    std::string const log = m_directory + "/build.log";
    int const         rc  = runProcess(
        {make, "-C", m_directory, "-j", std::to_string(nJobs), "lib"}, log);
    if (rc != 0)
        throw BuildError("Compilation of library '" + m_name
                         + "' failed (exit code " + std::to_string(rc)
                         + "), see " + log);
}

std::vector<LhaEntry> readLha(std::string const &path)
{
    std::ifstream in(path);
    if (!in)
        throw csl::api::LookupError("Cannot open LesHouches file '" + path
                                    + "'");
    mty::lha::LHAFileData data = mty::lha::Reader::readFile(in);

    constexpr std::size_t noSupIndex = static_cast<std::size_t>(-1);
    std::vector<LhaEntry> entries;
    for (std::size_t b = 0; b < data.size(); ++b) {
        mty::lha::LHABlock const &block = data[b];
        for (std::size_t e = 0; e < block.size(); ++e) {
            mty::lha::LHAElement const &element = block[e];
            LhaEntry                    entry{block.getName(),
                           {element.id},
                           static_cast<double>(element.value)};
            if (element.id_sup != noSupIndex)
                entry.indices.push_back(element.id_sup);
            entries.push_back(std::move(entry));
        }
    }
    return entries;
}

} // namespace mty::api::simulation
