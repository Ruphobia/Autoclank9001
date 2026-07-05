// SPDX-License-Identifier: GPL-3.0-or-later
// Smoke test for 016_safety: load seed dataset, evaluate a couple of
// realistic prompts, confirm the right entries fire, then test the
// per-project overlay mechanism.

#include "test_runner.hpp"
#include "../modules/016_safety/safety.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>

namespace {

namespace fs = std::filesystem;

fs::path scratch_root() {
    if (const char * e = std::getenv("TMPDIR")) return fs::path(e) / "tool_test_safety";
    return fs::path("/tmp") / "tool_test_safety";
}

template <typename V>
bool has_id(const V & v, const std::string & id) {
    for (const auto & e : v) if (e.id == id) return true;
    return false;
}

testing::TestOutcome run() {
    safety::init({});

    if (safety::hazards().empty()) return testing::fail("seed hazards empty (resources path?)");
    if (safety::legal().empty())   return testing::fail("seed legal empty (resources path?)");

    auto hf = safety::evaluate("I'm planning to etch a quartz part using HF.");
    if (!has_id(hf.hazards, "hf_skin")) return testing::fail("HF/etching did not match hf_skin");
    if (hf.preface.find("Hazards") == std::string::npos) return testing::fail("preface missing Hazards header");

    auto nfa = safety::evaluate("Considering building a suppressor for hearing protection.");
    if (!has_id(nfa.legal, "nfa_firearms_us")) return testing::fail("NFA topic missed");

    auto none = safety::evaluate("Help me sort a list of integers in C++.");
    if (!none.hazards.empty() || !none.legal.empty())
        return testing::fail("benign prompt produced advice");
    if (!none.preface.empty()) return testing::fail("benign prompt produced preface");

    // Overlay round trip: writing a same-id entry replaces the seed.
    fs::path root = scratch_root();
    std::error_code ec;
    fs::remove_all(root, ec);
    fs::create_directories(root / ".tool" / "safety");
    {
        std::ofstream o(root / ".tool" / "safety" / "local_hazards.json");
        o << R"({"entries":[{
            "id": "hf_skin",
            "material": "hydrofluoric acid",
            "synonyms": ["HF"],
            "process_keywords": ["etch"],
            "mechanism": "OVERLAY MECHANISM",
            "first_aid": "",
            "ppe": [],
            "severity": "test",
            "sources": []
        }]})";
    }
    safety::init(root.string());
    auto overlaid = safety::evaluate("Working with HF for etching today.");
    if (!has_id(overlaid.hazards, "hf_skin")) return testing::fail("overlay broke matching");
    bool saw_overlay = false;
    for (const auto & h : overlaid.hazards)
        if (h.id == "hf_skin" && h.mechanism == "OVERLAY MECHANISM") saw_overlay = true;
    if (!saw_overlay) return testing::fail("overlay did not replace the seed entry");

    safety::shutdown();
    fs::remove_all(root, ec);
    return testing::ok();
}

const int _r = testing::register_test(
    "safety",
    "016_safety: seed dataset, evaluate matches, project overlay precedence",
    &run);

}
