#include "marty/api/common/error.h"
#include "marty/api/modeling/modeling.h"
#include "marty/api/visualization/visualization.h"

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

namespace mod = mty::api::modeling;
namespace vis = mty::api::visualization;
namespace fs  = std::filesystem;

namespace {

// Compton scattering e A -> e A at tree level.
mod::Amplitude comptonAmplitude()
{
    auto model = mod::Model::qed();
    auto e     = model.particle("\\psi");
    auto A     = model.particle("A");
    return model.computeAmplitude(
        mod::Order::TreeLevel, {{e, true}, {e, false}, {A, true}, {A, false}});
}

fs::path tempFile(std::string const &name)
{
    return fs::temp_directory_path() / ("marty-vis-test-" + name);
}

} // namespace

TEST(VisualizationApi, ExplicitNonExecutableGrafedPathIsRejected)
{
    auto path = tempFile("not-executable");
    {
        std::ofstream out(path);
        out << "not a program\n";
    }
    fs::permissions(path, fs::perms::owner_read | fs::perms::owner_write);
    EXPECT_THROW(vis::findGrafed(path.string()), mty::api::ValueError);
    fs::remove(path);
}

TEST(VisualizationApi, DiagramsLatexIsStandaloneTikzDocument)
{
    auto latex = vis::diagramsLatex(comptonAmplitude());
    EXPECT_NE(latex.find("\\documentclass"), std::string::npos);
    EXPECT_NE(latex.find("tikz"), std::string::npos);
    EXPECT_NE(latex.find("\\end{document}"), std::string::npos);
}

TEST(VisualizationApi, SavedDiagramsAreOwnerReadWriteOnly)
{
    auto path = tempFile("diagrams.json");
    vis::saveDiagrams(comptonAmplitude(), path.string());
    auto perms = fs::status(path).permissions();
    EXPECT_EQ(perms & fs::perms::group_all, fs::perms::none);
    EXPECT_EQ(perms & fs::perms::others_all, fs::perms::none);
    fs::remove(path);
}

TEST(VisualizationApi, DiagramsJsonDescribesEveryDiagram)
{
    auto json = vis::diagramsJson(comptonAmplitude());
    EXPECT_FALSE(json.empty());
    EXPECT_NE(json.find("diagrams"), std::string::npos);
}
