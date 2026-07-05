// SPDX-License-Identifier: GPL-3.0-or-later
// Smoke test for 011_scripting: validate(), execute(), and
// save/load/list round trip. No model loads.

#include "test_runner.hpp"
#include "../modules/011_scripting/scripting.hpp"

#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <string>

namespace {

namespace fs = std::filesystem;

fs::path scratch_root() {
    // Prefer the harness-provided scratchpad; fall back to /tmp if unset
    // so ac9_test still runs under bare shells.
    if (const char * e = std::getenv("TMPDIR"))     return fs::path(e) / "tool_test_scripting";
    return fs::path("/tmp") / "tool_test_scripting";
}

testing::TestOutcome run() {
    scripting::init();

    if (!scripting::validate("echo hello").empty())
        return testing::fail("trivial echo flagged by validate()");

    if (scripting::validate("").empty())
        return testing::fail("empty script not flagged by validate()");

    auto warns = scripting::validate("rm -rf /");
    if (warns.empty())
        return testing::fail("rm -rf / not flagged by validate()");

    auto r = scripting::execute("printf hello; printf oops 1>&2; exit 3", "", 5);
    if (r.timed_out) return testing::fail("trivial script reported timeout");
    if (r.exit_code != 3)
        return testing::fail("expected exit 3, got " + std::to_string(r.exit_code) +
                             " (stderr=" + r.stderr_text + ")");
    if (r.stdout_text != "hello")
        return testing::fail("stdout mismatch: '" + r.stdout_text + "'");
    if (r.stderr_text.find("oops") == std::string::npos)
        return testing::fail("stderr did not contain 'oops': '" + r.stderr_text + "'");

    auto t = scripting::execute("sleep 5; echo late", "", 1);
    if (!t.timed_out) return testing::fail("expected timeout, ran to completion");
    if (t.stdout_text.find("late") != std::string::npos)
        return testing::fail("script kept running past timeout");

    fs::path root = scratch_root();
    std::error_code ec;
    fs::remove_all(root, ec);
    fs::create_directories(root);

    scripting::Script s;
    s.name        = "smoke_demo";
    s.description = "harness round trip";
    s.body        = "echo round-trip\n";
    scripting::save(s, root.string());

    auto names = scripting::list(root.string());
    if (names.size() != 1 || names[0] != "smoke_demo")
        return testing::fail("list() did not return the saved script");

    auto loaded = scripting::load("smoke_demo", root.string());
    if (loaded.body != s.body || loaded.description != s.description)
        return testing::fail("load() did not round trip the saved script");

    fs::remove_all(root, ec);
    scripting::shutdown();
    return testing::ok();
}

const int _r = testing::register_test(
    "scripting",
    "011_scripting: validate(), execute() with stdout/stderr/timeout, save/load/list",
    &run);

}
