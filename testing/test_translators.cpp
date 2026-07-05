// Smoke test for 014_format_translators: gzip/gunzip round trip,
// plan() returns nullopt for missing tools, and pairings are non-empty.

#include "test_runner.hpp"
#include "../modules/014_format_translators/translators.hpp"

#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>

namespace {

namespace fs = std::filesystem;

fs::path scratch_root() {
    if (const char * e = std::getenv("TMPDIR")) return fs::path(e) / "tool_test_translators";
    return fs::path("/tmp") / "tool_test_translators";
}

testing::TestOutcome run() {
    translators::init();

    if (translators::supported_pairings().empty())
        return testing::fail("supported_pairings empty");

    auto noplan = translators::plan("x.foo", "x.bar");
    if (noplan) return testing::fail("plan() returned a hit for unknown pair");

    if (!translators::tool_available("gzip"))
        return testing::skip("gzip not on PATH");
    if (!translators::tool_available("gunzip"))
        return testing::skip("gunzip not on PATH");

    fs::path root = scratch_root();
    std::error_code ec;
    fs::remove_all(root, ec);
    fs::create_directories(root);

    fs::path src = root / "hello.txt";
    fs::path gz  = root / "hello.txt.gz";
    fs::path out = root / "hello.txt.out";

    {
        std::ofstream o(src, std::ios::binary);
        o << "hello world\n";
    }

    auto p = translators::plan(src.string(), gz.string());
    if (!p) return testing::fail("plan() missed txt->gz");
    if (p->tool != "gzip") return testing::fail("plan() tool wrong: " + p->tool);

    auto r1 = translators::translate(src.string(), gz.string(), 10);
    if (r1.exit_code != 0)
        return testing::fail("gzip failed: " + std::to_string(r1.exit_code) +
                             " stderr=" + r1.stderr_text);
    if (!fs::exists(gz) || fs::file_size(gz) == 0)
        return testing::fail("gzip produced no output");

    auto r2 = translators::translate(gz.string(), out.string(), 10);
    if (r2.exit_code != 0)
        return testing::fail("gunzip failed: " + std::to_string(r2.exit_code) +
                             " stderr=" + r2.stderr_text);

    std::ifstream ri(out, std::ios::binary);
    std::ostringstream b; b << ri.rdbuf();
    if (b.str() != "hello world\n")
        return testing::fail("round trip mismatch: '" + b.str() + "'");

    fs::remove_all(root, ec);
    translators::shutdown();
    return testing::ok();
}

const int _r = testing::register_test(
    "translators",
    "014_format_translators: gzip/gunzip round trip + plan() lookups",
    &run);

}
