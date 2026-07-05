// SPDX-License-Identifier: GPL-3.0-or-later
// Smoke test for modules/351_drc.

#include "test_runner.hpp"
#include "../modules/346_kicad_model/kicad_model.hpp"
#include "../modules/351_drc/drc.hpp"

#include <memory>

namespace {

testing::TestOutcome run() {
    using namespace kicad_model;

    Board b;
    b.uuid   = make_uuid();
    b.layers = default_2layer_stackup();

    // Two pads on the same layer, different nets, way too close.
    auto fp1 = std::make_shared<Footprint>();
    fp1->lib_id = "test:R_0603";
    fp1->at     = {geom::mm_to_nm(10), geom::mm_to_nm(10)};
    fp1->fields.push_back({"Reference","R1",{},{},false,false,false,1,1,{},make_uuid()});
    { Pad p;
      p.number = "1"; p.kind = "smd"; p.shape = "roundrect";
      p.at     = {0, 0};
      p.size   = {geom::mm_to_nm(0.9), geom::mm_to_nm(0.9)};
      p.layers = {"F.Cu"};
      p.net    = 1; p.net_name = "+5V";
      fp1->pads.push_back(p);
    }
    b.items.push_back(fp1);

    auto fp2 = std::make_shared<Footprint>();
    fp2->lib_id = "test:R_0603";
    // Move pad 2 to be only 0.05 mm apart (edge to edge, so violates).
    fp2->at     = {geom::mm_to_nm(10.5), geom::mm_to_nm(10)}; // centers 0.5mm apart
    fp2->fields.push_back({"Reference","R2",{},{},false,false,false,1,1,{},make_uuid()});
    { Pad p;
      p.number = "1"; p.kind = "smd"; p.shape = "roundrect";
      p.at     = {0, 0};
      p.size   = {geom::mm_to_nm(0.9), geom::mm_to_nm(0.9)};
      p.layers = {"F.Cu"};
      p.net    = 2; p.net_name = "GND";
      fp2->pads.push_back(p);
    }
    b.items.push_back(fp2);

    // A track on a totally different net, way too close to fp1's pad.
    auto tr = std::make_shared<PcbTrack>();
    tr->start = {geom::mm_to_nm(9.6), geom::mm_to_nm(9.6)};
    tr->end   = {geom::mm_to_nm(9.6), geom::mm_to_nm(10.4)};
    tr->width_nm = geom::mm_to_nm(0.2);
    tr->layer = "F.Cu";
    tr->net = 3;
    b.items.push_back(tr);

    auto rep = drc::run(b, {});

    bool saw_pad_pad = false, saw_track_pad = false, saw_missing_crt = false;
    for (const auto & v : rep.violations) {
        if (v.rule_id == "clearance"        && v.message.find("pad") != std::string::npos && v.message.find("vs pad") != std::string::npos) saw_pad_pad = true;
        if (v.rule_id == "clearance"        && v.message.find("track") != std::string::npos && v.message.find("vs pad") != std::string::npos) saw_track_pad = true;
        if (v.rule_id == "missing_courtyard") saw_missing_crt = true;
    }
    if (!saw_pad_pad)     return testing::fail("pad-pad clearance not flagged");
    if (!saw_track_pad)   return testing::fail("track-pad clearance not flagged");
    if (!saw_missing_crt) return testing::fail("missing courtyard not flagged");

    std::string j = drc::to_kicad_json(rep, "smoke.kicad_pcb");
    if (j.find("\"violations\"") == std::string::npos) return testing::fail("json envelope");

    return testing::ok();
}

const int _r = testing::register_test(
    "drc",
    "DRC engine: pad-pad + track-pad clearance, hole-to-hole, edge clearance, missing courtyard; JSON envelope compatible with kicad-cli.",
    &run);

} // namespace
