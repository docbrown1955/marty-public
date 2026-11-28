// This file is part of MARTY.
//
// MARTY is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// MARTY is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with MARTY. If not, see <https://www.gnu.org/licenses/>.

#include "visualization.h"

#include "../common/error.h"
#include "../common/session.h"
#include "marty/core/amplitude.h"
#include "marty/core/drawer.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <spawn.h>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <sys/wait.h>
#include <system_error>
#include <unistd.h>
#include <vector>

#ifndef MTY_GRAFED_INSTALL_BIN
#define MTY_GRAFED_INSTALL_BIN ""
#endif

extern char **environ;

namespace mty::api::visualization {
namespace {

constexpr char const *grafedName = "grafed-gui";

bool isExecutable(std::string const &path)
{
    std::error_code ec;
    return !path.empty() && std::filesystem::is_regular_file(path, ec)
           && access(path.c_str(), X_OK) == 0;
}

std::string findInPath(std::string const &name)
{
    char const *path = std::getenv("PATH");
    if (!path)
        return "";
    std::istringstream dirs(path);
    std::string        dir;
    while (std::getline(dirs, dir, ':')) {
        if (dir.empty())
            continue;
        std::string candidate = dir + "/" + name;
        if (isExecutable(candidate))
            return candidate;
    }
    return "";
}

// Starts argv[0] searched in PATH. Standard output and error go to logPath,
// or to /dev/null when it is empty.
pid_t spawn(std::vector<std::string> const &args, std::string const &logPath)
{
    std::vector<char *> argv;
    for (auto const &arg : args)
        argv.push_back(const_cast<char *>(arg.c_str()));
    argv.push_back(nullptr);

    posix_spawn_file_actions_t actions;
    posix_spawn_file_actions_init(&actions);
    std::string sink = logPath.empty() ? "/dev/null" : logPath;
    posix_spawn_file_actions_addopen(&actions,
                                     STDOUT_FILENO,
                                     sink.c_str(),
                                     O_WRONLY | O_CREAT | O_TRUNC,
                                     0600);
    posix_spawn_file_actions_adddup2(&actions, STDOUT_FILENO, STDERR_FILENO);

    pid_t pid = 0;
    int   rc
        = posix_spawnp(&pid, argv[0], &actions, nullptr, argv.data(), environ);
    posix_spawn_file_actions_destroy(&actions);
    if (rc != 0)
        throw StateError("Cannot start '" + args[0] + "'.");
    return pid;
}

std::string tail(std::string const &file, std::size_t maxChars)
{
    std::ifstream      in(file);
    std::ostringstream text;
    text << in.rdbuf();
    std::string content = text.str();
    if (content.size() > maxChars)
        content = "..." + content.substr(content.size() - maxChars);
    return content;
}

std::string readFile(std::string const &file)
{
    std::ifstream      in(file, std::ios::binary);
    std::ostringstream bytes;
    bytes << in.rdbuf();
    return bytes.str();
}

} // namespace

std::string diagramsJson(modeling::Amplitude const &amplitude)
{
    std::string templ
        = (std::filesystem::temp_directory_path() / "pymarty-XXXXXX").string();
    std::vector<char> path(templ.begin(), templ.end());
    path.push_back('\0');
    int fd = mkstemp(path.data());
    if (fd == -1)
        throw Error("Cannot create a temporary file for the JSON export.");
    close(fd);
    std::string file(path.data());

    guarded([&] {
        Session session;
        mty::Drawer::saveToJSON(file, amplitude.raw().obtainGraphs());
    });
    std::string text = readFile(file);
    std::remove(file.c_str());
    return text;
}

void saveDiagrams(modeling::Amplitude const &amplitude,
                  std::string const         &path)
{
    std::string text = diagramsJson(amplitude);
    int         fd
        = open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd == -1)
        throw ValueError("Cannot write the diagrams to '" + path + "'.");
    std::size_t written = 0;
    while (written < text.size()) {
        ssize_t n = write(fd, text.data() + written, text.size() - written);
        if (n <= 0) {
            close(fd);
            throw ValueError("Cannot write the diagrams to '" + path + "'.");
        }
        written += static_cast<std::size_t>(n);
    }
    close(fd);
}

static std::string standaloneDocument(std::string const &blocks)
{
    return "\\documentclass[preview,border=5pt]{standalone}\n"
           "\\usepackage{tikz-feynman}\n"
           "\\begin{document}\n"
           + blocks + "\\end{document}\n";
}

std::string diagramsLatex(modeling::Amplitude const &amplitude)
{
    std::string blocks;
    guarded([&] {
        Session session;
        for (auto const &graph : amplitude.raw().obtainGraphs())
            blocks += mty::Drawer::buildLatex(graph) + "\\vspace{1em}\n";
    });
    return standaloneDocument(blocks);
}

std::string rulesLatex(modeling::FeynmanRules const &rules)
{
    return standaloneDocument(rules.latexDocument());
}

std::string renderPng(std::string const &latexDocument)
{
    std::string latex  = findInPath("latex");
    std::string dvipng = findInPath("dvipng");
    if (latex.empty() || dvipng.empty())
        throw StateError("Rendering diagrams needs 'latex' and 'dvipng' on "
                         "the PATH, with "
                         "the LaTeX packages 'standalone' and "
                         "'tikz-feynman'.");

    std::string templ
        = (std::filesystem::temp_directory_path() / "pymarty-render-XXXXXX")
              .string();
    std::vector<char> dir(templ.begin(), templ.end());
    dir.push_back('\0');
    if (mkdtemp(dir.data()) == nullptr)
        throw Error("Cannot create a temporary directory for the rendering.");
    std::string work(dir.data());
    struct Cleanup {
        std::string dir;
        ~Cleanup()
        {
            std::error_code ec;
            std::filesystem::remove_all(dir, ec);
        }
    } cleanup{work};

    std::string tex = work + "/diagrams.tex";
    {
        std::ofstream out(tex);
        out << latexDocument;
    }
    pid_t pid    = spawn({latex,
                          "-interaction=nonstopmode",
                          "-halt-on-error",
                          "-output-directory=" + work,
                          tex},
                      work + "/latex.log");
    int   status = waitProcess(pid);
    if (status != 0)
        throw StateError("LaTeX failed to compile the diagrams:\n"
                         + tail(work + "/latex.log", 2000));

    std::string png = work + "/diagrams.png";
    pid             = spawn({dvipng,
                             "-T",
                             "tight",
                             "-D",
                             "200",
                             "-bg",
                             "Transparent",
                             "-o",
                             png,
                             work + "/diagrams.dvi"},
                work + "/dvipng.log");
    if (waitProcess(pid) != 0)
        throw StateError("dvipng failed to convert the diagrams:\n"
                         + tail(work + "/dvipng.log", 2000));
    return readFile(png);
}

std::string findGrafed(std::string const &explicitPath)
{
    if (!explicitPath.empty()) {
        if (!isExecutable(explicitPath))
            throw ValueError("'" + explicitPath
                             + "' is not an executable file.");
        return explicitPath;
    }
    if (char const *env = std::getenv("MARTY_GRAFED"); env && *env) {
        if (isExecutable(env))
            return env;
    }
    if (std::string found = findInPath(grafedName); !found.empty())
        return found;
    std::string installed = std::string(MTY_GRAFED_INSTALL_BIN);
    if (!installed.empty()) {
        std::string candidate = installed + "/" + grafedName;
        if (isExecutable(candidate))
            return candidate;
    }
    return "";
}

pid_t launchGrafed(std::string const &executable, std::string const &jsonPath)
{
    return spawn({executable, jsonPath}, "");
}

int waitProcess(pid_t pid)
{
    int status = 0;
    if (waitpid(pid, &status, 0) == -1)
        throw StateError("Cannot wait for process " + std::to_string(pid)
                         + ".");
    return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
}

} // namespace mty::api::visualization
