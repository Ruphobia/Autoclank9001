// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/346_kicad_model/kicad_model.hpp"
#include "../modules/362_board_setup/board_setup.hpp"

namespace {

testing::TestOutcome run() {
    using namespace kicad_model;
    Board b; b.uuid = make_uuid(); b.layers = default_2layer_stackup();
    b.thickness_mm = 1.6;
    // Prime the raw_setup_sexpr with some fields.
    b.raw_setup_sexpr =
        "(setup\n"
        "  (min_clearance 0.20)\n"
        "  (min_track_width 0.18)\n"
        "  (allow_microvias yes)\n"
        ")\n";

    auto s = board_setup::extract(b);
    if (s.stackup.layers.empty())             return testing::fail("no layers");
    if (std::abs(s.min_clearance_mm - 0.20)   > 1e-9) return testing::fail("min_clearance parse");
    if (std::abs(s.min_track_width_mm - 0.18) > 1e-9) return testing::fail("min_track_width parse");
    if (!s.allow_microvias)                   return testing::fail("microvias parse");
    if (s.classes.empty() || s.classes[0].name != "Default") return testing::fail("default class");

    // Modify + apply.
    s.min_clearance_mm = 0.25;
    s.allow_blind_buried_vias = true;
    board_setup::apply(b, s);
    if (b.raw_setup_sexpr.find("(min_clearance 0.25)") == std::string::npos)
        return testing::fail("apply did not update min_clearance");
    if (b.raw_setup_sexpr.find("(allow_blind_buried_vias yes)") == std::string::npos)
        return testing::fail("apply did not update blind_buried");

    // JSON round-trip.
    std::string j = board_setup::to_json(s);
    auto s2 = board_setup::from_json(j);
    if (std::abs(s2.min_clearance_mm - s.min_clearance_mm) > 1e-9) return testing::fail("json round-trip clearance");
    if (s2.stackup.layers.size() != s.stackup.layers.size())        return testing::fail("json round-trip layers count");
    if (s2.classes.size()        != s.classes.size())               return testing::fail("json round-trip classes count");

    return testing::ok();
}

const int _r = testing::register_test(
    "board_setup",
    "Setup: parse raw setup sexpr, mutate, re-emit; JSON round-trip.",
    &run);

} // namespace
