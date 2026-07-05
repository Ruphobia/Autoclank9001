// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/346_kicad_model/kicad_model.hpp"
#include "../modules/369_pns_shove/pns_shove.hpp"

#include <memory>

namespace {

testing::TestOutcome run() {
    using namespace kicad_model;
    Board b; b.uuid = make_uuid(); b.layers = default_2layer_stackup();
    intern_net(b, ""); intern_net(b, "N1"); intern_net(b, "N2"); intern_net(b, "N3");

    // Existing horizontal track on N2.
    auto t = std::make_shared<PcbTrack>();
    t->start    = {geom::mm_to_nm(0),  geom::mm_to_nm(10)};
    t->end      = {geom::mm_to_nm(20), geom::mm_to_nm(10)};
    t->width_nm = geom::mm_to_nm(0.2);
    t->layer    = "F.Cu";
    t->net      = 2;
    t->uuid     = make_uuid();
    b.items.push_back(t);

    // Propose a track on N1 that would cross the existing one.
    PcbTrack prop;
    prop.start    = {geom::mm_to_nm(10), geom::mm_to_nm(5)};
    prop.end      = {geom::mm_to_nm(10), geom::mm_to_nm(15)};
    prop.width_nm = geom::mm_to_nm(0.2);
    prop.layer    = "F.Cu";
    prop.net      = 1;

    auto res = pns_shove::add_track(b, prop);
    // On crossing configuration, shove attempts to move the horizontal
    // track vertically to make room. Depending on ordering, this may
    // succeed or report unshoveable. Either outcome is OK for the smoke
    // test; what we assert is that on failure no state was changed.
    if (!res.ok) {
        // Verify no leftover shove was applied.
        auto after = static_cast<PcbTrack*>(b.items[0].get());
        if (after->start.y != geom::mm_to_nm(10) || after->end.y != geom::mm_to_nm(10))
            return testing::fail("failed shove left state modified");
    } else {
        if (res.new_track_index != 1) return testing::fail("new_track_index");
    }

    // Now try a completely clear proposal on a fresh board: should succeed.
    Board b2; b2.uuid = make_uuid(); b2.layers = default_2layer_stackup();
    intern_net(b2, ""); intern_net(b2, "X");
    PcbTrack pp;
    pp.start    = {0, 0};
    pp.end      = {geom::mm_to_nm(30), 0};
    pp.width_nm = geom::mm_to_nm(0.2);
    pp.layer    = "F.Cu";
    pp.net      = 1;
    auto r2 = pns_shove::add_track(b2, pp);
    if (!r2.ok) return testing::fail("clear placement should succeed: " + r2.reason);
    if (b2.items.size() != 1) return testing::fail("track not added");

    return testing::ok();
}

const int _r = testing::register_test(
    "pns_shove",
    "Simplified push-and-shove: on-collision perpendicular shove with recursion cap; clear placement always succeeds.",
    &run);

} // namespace
