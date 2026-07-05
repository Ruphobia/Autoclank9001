// SPDX-License-Identifier: GPL-3.0-or-later
// Smoke test for 073_git_workbench.

#include "test_runner.hpp"
#include "../modules/073_git_workbench/git_workbench.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>

namespace {

namespace fs = std::filesystem;

fs::path scratch_root() {
    if (const char * e = std::getenv("TMPDIR")) return fs::path(e) / "tool_test_git";
    return fs::path("/tmp") / "tool_test_git";
}

testing::TestOutcome run() {
    git_workbench::init();
    if (!git_workbench::git_available())
        return testing::skip("git not on PATH");

    fs::path root = scratch_root();
    std::error_code ec;
    fs::remove_all(root, ec);
    fs::create_directories(root);

    // git init + initial commit so we have a working repo.
    std::string init = std::string("cd '") + root.string() +
        "' && git init -q && git config user.email t@t && git config user.name tester && "
        "git config commit.gpgsign false && "
        "echo hello > a.txt && git add a.txt && git commit -q -m 'initial' 2>&1";
    if (std::system(init.c_str()) != 0)
        return testing::fail("git init scratch repo failed");

    auto r_root = git_workbench::repo_root(root.string());
    if (!r_root) return testing::fail("repo_root returned nullopt");

    // Clean status after the initial commit.
    auto s0 = git_workbench::status(root.string());
    if (!s0.staged.empty() || !s0.unstaged.empty() || !s0.untracked.empty())
        return testing::fail("status after initial commit not clean");
    if (s0.branch.empty()) return testing::fail("branch field empty");

    // Modify + stage; expect 1 staged entry.
    {
        std::ofstream o(root / "a.txt", std::ios::app);
        o << "second line\n";
    }
    git_workbench::stage(root.string(), {"a.txt"});
    auto s1 = git_workbench::status(root.string());
    if (s1.staged.size() != 1 || s1.staged[0].path != "a.txt")
        return testing::fail("staged entry missing after add");

    // commit + log.
    auto cm = git_workbench::commit_msg(root.string(), "modify a.txt");
    if (cm.exit_code != 0)
        return testing::fail("commit failed: " + cm.stderr_text);
    auto lg = git_workbench::log(root.string(), "-5");
    if (lg.size() != 2)
        return testing::fail("log expected 2 commits, got " + std::to_string(lg.size()));
    if (lg[0].subject != "modify a.txt")
        return testing::fail("log top subject wrong: " + lg[0].subject);

    // Untracked file shows up in status.untracked.
    {
        std::ofstream o(root / "b.txt"); o << "fresh\n";
    }
    auto s2 = git_workbench::status(root.string());
    bool seen_b = false;
    for (const auto & u : s2.untracked) if (u == "b.txt") seen_b = true;
    if (!seen_b) return testing::fail("untracked file missing from status");

    // Branch create + checkout.
    auto cb = git_workbench::create_branch(root.string(), "feature/x");
    if (cb.exit_code != 0)
        return testing::fail("create_branch failed");
    auto co = git_workbench::checkout(root.string(), "feature/x");
    if (co.exit_code != 0)
        return testing::fail("checkout failed: " + co.stderr_text);
    if (git_workbench::current_branch(root.string()) != "feature/x")
        return testing::fail("current_branch did not flip");

    auto bs = git_workbench::branches(root.string());
    bool saw_feat = false;
    for (const auto & b : bs) {
        if (b.name == "feature/x" && b.current) saw_feat = true;
    }
    if (!saw_feat) return testing::fail("branches did not list feature/x as current");

    // Diff of an unstaged change.
    {
        std::ofstream o(root / "a.txt", std::ios::app);
        o << "third line\n";
    }
    auto d = git_workbench::diff(root.string());
    if (d.find("third line") == std::string::npos)
        return testing::fail("diff missed third line");

    fs::remove_all(root, ec);
    git_workbench::shutdown();
    return testing::ok();
}

const int _r = testing::register_test(
    "git_workbench",
    "073_git_workbench: status / log / branch / commit / diff",
    &run);

}
