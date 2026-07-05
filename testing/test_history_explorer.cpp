// SPDX-License-Identifier: GPL-3.0-or-later
// Smoke test for 074_history_explorer.

#include "test_runner.hpp"
#include "../modules/074_history_explorer/history_explorer.hpp"
#include "../modules/073_git_workbench/git_workbench.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>

namespace {

namespace fs = std::filesystem;

fs::path scratch_root() {
    if (const char * e = std::getenv("TMPDIR")) return fs::path(e) / "tool_test_history";
    return fs::path("/tmp") / "tool_test_history";
}

testing::TestOutcome run() {
    history_explorer::init();
    if (!git_workbench::git_available())
        return testing::skip("git not on PATH");

    fs::path root = scratch_root();
    std::error_code ec;
    fs::remove_all(root, ec);
    fs::create_directories(root);

    // Build a tiny repo with three commits.
    std::string setup = std::string("cd '") + root.string() +
        "' && git init -q && "
        "git config user.email t@t && git config user.name tester && "
        "git config commit.gpgsign false && "
        "echo first > a.txt && git add a.txt && git commit -q -m 'add a.txt' && "
        "echo second >> a.txt && git commit -q -am 'append second' && "
        "echo capybara >> a.txt && git commit -q -am 'add capybara marker'";
    if (std::system(setup.c_str()) != 0)
        return testing::fail("scratch repo setup failed");

    auto blame = history_explorer::blame(root.string(), "a.txt");
    if (blame.size() != 3) return testing::fail("blame line count != 3");
    if (blame[0].text != "first")    return testing::fail("blame line 1 wrong: " + blame[0].text);
    if (blame[2].text != "capybara") return testing::fail("blame line 3 wrong: " + blame[2].text);
    if (blame[0].sha.empty() || blame[2].sha.empty())
        return testing::fail("blame missing sha");
    if (blame[0].author != "tester") return testing::fail("blame author wrong");

    auto fh = history_explorer::file_history(root.string(), "a.txt");
    if (fh.size() != 3) return testing::fail("file history count != 3");
    if (fh[0].subject != "add capybara marker")
        return testing::fail("file history newest first wrong");

    auto pk = history_explorer::pickaxe(root.string(), "capybara");
    if (pk.size() != 1 || pk[0].subject != "add capybara marker")
        return testing::fail("pickaxe term not located");

    auto by_a = history_explorer::commits_by_author(root.string());
    if (by_a.empty() || by_a[0].author != "tester" || by_a[0].count != 3)
        return testing::fail("commits_by_author wrong");

    // Single-line blame.
    auto one = history_explorer::blame(root.string(), "a.txt", 2);
    if (one.size() != 1 || one[0].text != "second")
        return testing::fail("single-line blame wrong");

    fs::remove_all(root, ec);
    history_explorer::shutdown();
    return testing::ok();
}

const int _r = testing::register_test(
    "history_explorer",
    "074_history_explorer: blame + file_history + pickaxe + commits_by_author",
    &run);

}
