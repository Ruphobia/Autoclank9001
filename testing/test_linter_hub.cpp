// SPDX-License-Identifier: GPL-3.0-or-later
// Smoke test for 072_linter_hub.

#include "test_runner.hpp"
#include "../modules/072_linter_hub/linter_hub.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>

namespace {

namespace fs = std::filesystem;

testing::TestOutcome run() {
    linter_hub::init();

    if (linter_hub::supported_pairings().empty())
        return testing::fail("supported_pairings empty");

    // plan() for an unknown extension returns nullopt.
    if (linter_hub::plan("x.unknownext", linter_hub::Mode::Lint))
        return testing::fail("plan() returned a hit for unknown extension");

    // gofmt is on PATH in standard Linux setups; if not, we skip.
    if (!linter_hub::tool_available("gofmt"))
        return testing::skip("gofmt not on PATH");

    fs::path tmpdir = std::getenv("TMPDIR")
                      ? fs::path(std::getenv("TMPDIR")) / "tool_test_linthub"
                      : fs::path("/tmp/tool_test_linthub");
    std::error_code ec;
    fs::remove_all(tmpdir, ec);
    fs::create_directories(tmpdir);

    fs::path src = tmpdir / "hello.go";
    {
        std::ofstream o(src);
        // Bad formatting: missing indentation.
        o << "package main\nfunc main(){\nprintln(\"hi\")\n}\n";
    }

    auto p = linter_hub::plan(src.string(), linter_hub::Mode::Format);
    if (!p) return testing::fail("plan() missed go format");
    if (p->tool != "gofmt") return testing::fail("plan tool wrong");

    auto r = linter_hub::run(src.string(), linter_hub::Mode::Format, 10);
    if (r.exit_code != 0)
        return testing::fail("gofmt run failed: " + std::to_string(r.exit_code) +
                             " stderr=" + r.stderr_text);

    // gofmt -w rewrites in place; the file should now have a tab indent.
    std::ifstream in(src);
    std::string body((std::istreambuf_iterator<char>(in)),
                     std::istreambuf_iterator<char>());
    if (body.find("\tprintln") == std::string::npos)
        return testing::fail("gofmt did not reformat: '" + body + "'");

    fs::remove_all(tmpdir, ec);
    linter_hub::shutdown();
    return testing::ok();
}

const int _r = testing::register_test(
    "linter_hub",
    "072_linter_hub: pairings + gofmt-driven format run",
    &run);

}
