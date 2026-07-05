// SPDX-License-Identifier: GPL-3.0-or-later
// Smoke test for modules/355_pcb_ops.

#include "test_runner.hpp"
#include "../modules/346_kicad_model/kicad_model.hpp"
#include "../modules/355_pcb_ops/pcb_ops.hpp"

#include <memory>

namespace {

testing::TestOutcome run() {
    using namespace kicad_model;

    Board b;
    b.uuid = make_uuid();
    b.layers = default_2layer_stackup();

    // Two footprints on the same net + a track between them.
    auto fp1 = std::make_shared<Footprint>();
    fp1->lib_id = "test:R";
    fp1->at = {geom::mm_to_nm(10), geom::mm_to_nm(10)};
    fp1->fields.push_back({"Reference","R1",{},{},false,false,false,1,1,{},make_uuid()});
    { Pad p; p.number="1"; p.kind="smd"; p.shape="rect"; p.at={0,0}; p.size={geom::mm_to_nm(1), geom::mm_to_nm(1)}; p.layers={"F.Cu"}; p.net=5; p.net_name="+5V"; fp1->pads.push_back(p); }
    fp1->uuid = make_uuid();
    b.items.push_back(fp1);

    auto fp2 = std::make_shared<Footprint>();
    fp2->lib_id = "test:R";
    fp2->at = {geom::mm_to_nm(30), geom::mm_to_nm(10)};
    fp2->fields.push_back({"Reference","R2",{},{},false,false,false,1,1,{},make_uuid()});
    { Pad p; p.number="1"; p.kind="smd"; p.shape="rect"; p.at={0,0}; p.size={geom::mm_to_nm(1), geom::mm_to_nm(1)}; p.layers={"F.Cu"}; p.net=5; p.net_name="+5V"; fp2->pads.push_back(p); }
    fp2->uuid = make_uuid();
    b.items.push_back(fp2);

    auto t = std::make_shared<PcbTrack>();
    t->start = fp1->at;
    t->end   = fp2->at;
    t->width_nm = geom::mm_to_nm(0.2);
    t->layer = "F.Cu";
    t->net = 5;
    t->uuid = make_uuid();
    b.items.push_back(t);

    // Teardrops.
    auto before = b.items.size();
    auto n = pcb_ops::generate_teardrops(b, {});
    if (n < 2) return testing::fail("expected >=2 teardrops");
    if (b.items.size() != before + n) return testing::fail("teardrops not appended");

    // Cross-probe.
    auto u = pcb_ops::pcb_uuids_for_ref(b, "R2");
    if (u.size() != 1 || u[0] != fp2->uuid) return testing::fail("cross probe R2");

    // Align two footprints on X.
    pcb_ops::align(b, { fp1->uuid, fp2->uuid }, pcb_ops::AlignAxis::CenterX);
    // After center-X align, both x coords equal.
    Footprint * f1 = static_cast<Footprint*>(b.items[0].get());
    Footprint * f2 = static_cast<Footprint*>(b.items[1].get());
    if (f1->at.x != f2->at.x) return testing::fail("align CenterX did not equalize");

    return testing::ok();
}

const int _r = testing::register_test(
    "pcb_ops",
    "Teardrop generator + align/distribute + cross-probe lookups.",
    &run);

} // namespace
