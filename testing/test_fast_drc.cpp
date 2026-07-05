// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/346_kicad_model/kicad_model.hpp"
#include "../modules/365_fast_drc/fast_drc.hpp"

#include <memory>

namespace {

testing::TestOutcome run() {
    using namespace kicad_model;
    Board b; b.uuid = make_uuid(); b.layers = default_2layer_stackup();
    intern_net(b, ""); intern_net(b, "N1"); intern_net(b, "N2");

    auto fp = std::make_shared<Footprint>();
    fp->lib_id = "test:R";
    fp->at = {geom::mm_to_nm(10), geom::mm_to_nm(10)};
    fp->fields.push_back({"Reference","R1",{},{},false,false,false,1,1,{},make_uuid()});
    { Pad p; p.number="1"; p.kind="smd"; p.shape="rect"; p.at={0,0};
      p.size = {geom::mm_to_nm(1),geom::mm_to_nm(1)};
      p.layers={"F.Cu"};
      p.net=1; fp->pads.push_back(p);
    }
    fp->uuid = make_uuid();
    b.items.push_back(fp);

    // A proposed track on a different net that passes right next to the pad.
    fast_drc::ProposedTrack pt;
    pt.start = {geom::mm_to_nm(9.6), geom::mm_to_nm(9.6)};
    pt.end   = {geom::mm_to_nm(9.6), geom::mm_to_nm(10.4)};
    pt.width_nm = geom::mm_to_nm(0.2);
    pt.layer = "F.Cu";
    pt.net = 2;

    auto hits = fast_drc::check_track(b, pt, {});
    if (hits.empty()) return testing::fail("expected clearance collision");

    // Same net: no collision.
    pt.net = 1;
    auto hits2 = fast_drc::check_track(b, pt, {});
    if (!hits2.empty()) return testing::fail("same net should not collide");

    // Via check: hole-to-hole against pad drill.
    auto fp2 = std::make_shared<Footprint>();
    fp2->lib_id = "test:pth";
    fp2->at = {geom::mm_to_nm(20), geom::mm_to_nm(20)};
    fp2->fields.push_back({"Reference","J1",{},{},false,false,false,1,1,{},make_uuid()});
    { Pad p; p.number="1"; p.kind="thru_hole"; p.shape="circle"; p.at={0,0};
      p.size = {geom::mm_to_nm(1.5),geom::mm_to_nm(1.5)};
      p.drill_nm = geom::mm_to_nm(0.8);
      p.layers = {"*.Cu"};
      p.net = 1;
      fp2->pads.push_back(p);
    }
    fp2->uuid = make_uuid();
    b.items.push_back(fp2);

    fast_drc::ProposedVia pv;
    pv.at = {geom::mm_to_nm(20.15), geom::mm_to_nm(20)};
    pv.size_nm  = geom::mm_to_nm(0.6);
    pv.drill_nm = geom::mm_to_nm(0.3);
    pv.net = 2;

    auto hits3 = fast_drc::check_via(b, pv, {});
    bool saw_hh = false, saw_cl = false;
    for (const auto & c : hits3) {
        if (c.reason == "hole_to_hole") saw_hh = true;
        if (c.reason == "clearance")    saw_cl = true;
    }
    if (!saw_hh) return testing::fail("expected hole_to_hole collision");
    if (!saw_cl) return testing::fail("expected clearance collision");

    return testing::ok();
}

const int _r = testing::register_test(
    "fast_drc",
    "Live per-item DRC: track vs pad clearance, same-net skip, via hole-to-hole vs through-hole pad drill.",
    &run);

} // namespace
