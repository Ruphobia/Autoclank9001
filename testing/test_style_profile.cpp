// SPDX-License-Identifier: GPL-3.0-or-later
// Smoke test for 028_style_profile.

#include "test_runner.hpp"
#include "../modules/028_style_profile/style_profile.hpp"

#include <cstdlib>
#include <filesystem>

namespace {

namespace fs = std::filesystem;

fs::path scratch_root() {
    if (const char * e = std::getenv("TMPDIR")) return fs::path(e) / "tool_test_style";
    return fs::path("/tmp") / "tool_test_style";
}

testing::TestOutcome run() {
    fs::path root = scratch_root();
    std::error_code ec;
    fs::remove_all(root, ec);
    fs::create_directories(root);

    style_profile::init();

    // Missing file: default profile, empty prompt.
    auto def = style_profile::load(root.string());
    if (!def.tone.empty() || !def.avoid.empty())
        return testing::fail("default profile not empty");
    if (!style_profile::format_system_prompt(def).empty())
        return testing::fail("empty profile produced prompt");

    style_profile::Profile p;
    p.tone     = "terse";
    p.pronouns = "they/them";
    p.units_preferred = {"metric","SI"};
    p.avoid           = {"em dashes","emoji","hedging preambles"};
    p.use             = {"code fences","inline citations"};
    p.notes           = "User dislikes em dashes hard.";
    style_profile::save(p, root.string());

    auto path = style_profile::profile_path(root.string());
    if (!fs::exists(path)) return testing::fail("save did not create file");

    auto round = style_profile::load(root.string());
    if (round.tone != "terse" || round.pronouns != "they/them")
        return testing::fail("round trip lost header fields");
    if (round.avoid.size() != 3 || round.use.size() != 2 ||
        round.units_preferred.size() != 2)
        return testing::fail("round trip lost bullet sections");
    if (round.notes.find("dislikes em dashes") == std::string::npos)
        return testing::fail("round trip lost notes");

    auto sp = style_profile::format_system_prompt(round);
    if (sp.find("terse tone") == std::string::npos)
        return testing::fail("prompt missing tone clause");
    if (sp.find("Avoid: em dashes") == std::string::npos)
        return testing::fail("prompt missing avoid list");
    if (sp.find("metric") == std::string::npos)
        return testing::fail("prompt missing units");
    if (sp.find("they/them") == std::string::npos)
        return testing::fail("prompt missing pronouns");

    style_profile::shutdown();
    fs::remove_all(root, ec);
    return testing::ok();
}

const int _r = testing::register_test(
    "style_profile",
    "028_style_profile: load/save/round trip + system-prompt fragment",
    &run);

}
