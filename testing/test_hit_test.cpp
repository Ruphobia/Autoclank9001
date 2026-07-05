// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/346_kicad_model/kicad_model.hpp"
#include "../modules/379_hit_test/hit_test.hpp"

#include <memory>

namespace {

testing::TestOutcome run() {
    using namespace kicad_model;
    Schematic sch; sch.uuid = make_uuid();
    auto sym = std::make_shared<SchSymbol>();
    sym->lib_id = "Device:R";
    sym->at = { geom::mm_to_nm(50), geom::mm_to_nm(50) };
    sym->uuid = make_uuid();
    sch.root.items.push_back(sym);

    auto hits = hit_test::pick_sch(sch, 51.0, 51.0);
    if (hits.empty()) return testing::fail("pick failed near symbol");
    if (hits[0].uuid != sym->uuid) return testing::fail("wrong uuid");

    // Point far away.
    auto miss = hit_test::pick_sch(sch, 200.0, 200.0);
    if (!miss.empty()) return testing::fail("false positive");

    // Box select over the symbol.
    auto sel = hit_test::select_sch(sch, 40, 40, 60, 60);
    if (sel.empty()) return testing::fail("box select missed");

    // PCB side.
    Board b; b.uuid = make_uuid(); b.layers = default_2layer_stackup();
    auto fp = std::make_shared<Footprint>();
    fp->lib_id = "test:R"; fp->at = { geom::mm_to_nm(20), geom::mm_to_nm(30) };
    fp->uuid = make_uuid();
    b.items.push_back(fp);
    auto t = std::make_shared<PcbTrack>();
    t->start = { geom::mm_to_nm(20), geom::mm_to_nm(30) };
    t->end   = { geom::mm_to_nm(30), geom::mm_to_nm(30) };
    t->width_nm = geom::mm_to_nm(0.2); t->layer = "F.Cu"; t->net = 1;
    t->uuid = make_uuid();
    b.items.push_back(t);

    auto ph = hit_test::pick_pcb(b, 25.0, 30.0);
    if (ph.empty()) return testing::fail("pcb pick track");

    return testing::ok();
}

const int _r = testing::register_test(
    "hit_test",
    "Server-side hit-test: point pick + box select on schematic + PCB items.",
    &run);

} // namespace
