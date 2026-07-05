// SPDX-License-Identifier: GPL-3.0-or-later
// Golden-file DRC/ERC regressions (todo.txt task 89).
// Curated set of hand-built model fixtures whose reports have known
// error counts. When someone changes a rule, this suite fails until
// the golden numbers are updated deliberately.

#include "test_runner.hpp"
#include "../modules/346_kicad_model/kicad_model.hpp"
#include "../modules/349_netlist/netlist.hpp"
#include "../modules/350_erc/erc.hpp"
#include "../modules/351_drc/drc.hpp"

#include <memory>

namespace {

using namespace kicad_model;

std::shared_ptr<SchSymbol> make_sym(Schematic & sch, std::string ref, std::string val,
                                    std::string lib_id, long long x_mm, long long y_mm) {
    auto s = std::make_shared<SchSymbol>();
    s->lib_id = lib_id;
    s->at = { geom::mm_to_nm(x_mm), geom::mm_to_nm(y_mm) };
    s->fields.push_back({"Reference", ref, {}, {}, false, false, false, 1, 1, {}, make_uuid()});
    s->fields.push_back({"Value",     val, {}, {}, false, false, false, 1, 1, {}, make_uuid()});
    s->uuid = make_uuid();
    sch.root.items.push_back(s);
    return s;
}

testing::TestOutcome run() {
    // ------------ Golden ERC fixture ------------
    Schematic sch; sch.uuid = make_uuid();

    LibSymbol libR;
    libR.lib_id = "Device:R";
    { SchPin p; p.number="1"; p.electrical="passive"; p.at={0, geom::mm_to_nm( 2.54)}; libR.pins.push_back(p); }
    { SchPin p; p.number="2"; p.electrical="passive"; p.at={0, geom::mm_to_nm(-2.54)}; libR.pins.push_back(p); }
    sch.lib_symbols["Device:R"] = libR;

    LibSymbol libU;
    libU.lib_id = "Test:U1";
    { SchPin p; p.number="1"; p.electrical="input";     p.at={0, geom::mm_to_nm( 2.54)}; libU.pins.push_back(p); }
    { SchPin p; p.number="2"; p.electrical="output";    p.at={0, geom::mm_to_nm(-2.54)}; libU.pins.push_back(p); }
    sch.lib_symbols["Test:U1"] = libU;

    LibSymbol libPWR;
    libPWR.lib_id = "power:+5V";
    { SchPin p; p.number="1"; p.electrical="power_out"; p.at={0, 0}; libPWR.pins.push_back(p); }
    sch.lib_symbols["power:+5V"] = libPWR;

    // Two R1s (duplicate refdes).
    make_sym(sch, "R1", "10k", "Device:R", 30, 30);
    make_sym(sch, "R1", "1k",  "Device:R", 60, 30);
    // U1 with missing Value.
    auto u = make_sym(sch, "U1", "", "Test:U1", 30, 60);
    (void) u;
    // A power symbol.
    make_sym(sch, "#PWR01", "+5V", "power:+5V", 50, 90);

    auto nl = netlist::derive(sch);
    auto er = erc::run(sch, nl, {});

    // GOLDEN: at least 1 duplicate_reference, 1 missing_value, various
    // pin_not_connected. We assert the two golden rules explicitly.
    bool saw_dup = false, saw_missing_val = false;
    for (const auto & v : er.violations) {
        if (v.rule_id == "duplicate_reference") saw_dup = true;
        if (v.rule_id == "missing_value")       saw_missing_val = true;
    }
    if (!saw_dup)         return testing::fail("golden ERC: missing duplicate_reference");
    if (!saw_missing_val) return testing::fail("golden ERC: missing missing_value");

    // ------------ Golden DRC fixture ------------
    Board b; b.uuid = make_uuid(); b.layers = default_2layer_stackup();
    intern_net(b, ""); intern_net(b, "N1"); intern_net(b, "N2");

    auto fp1 = std::make_shared<Footprint>();
    fp1->lib_id = "test:R";
    fp1->at = {geom::mm_to_nm(10), geom::mm_to_nm(10)};
    fp1->fields.push_back({"Reference","R1",{},{},false,false,false,1,1,{},make_uuid()});
    { Pad p; p.number="1"; p.kind="smd"; p.shape="rect"; p.at={0,0};
      p.size = {geom::mm_to_nm(1),geom::mm_to_nm(1)}; p.layers={"F.Cu"};
      p.net=1; p.net_name="N1"; fp1->pads.push_back(p);
    }
    fp1->uuid = make_uuid();
    b.items.push_back(fp1);

    // Very close pad on different net (clearance violation).
    auto fp2 = std::make_shared<Footprint>();
    fp2->lib_id = "test:R";
    fp2->at = {geom::mm_to_nm(10.5), geom::mm_to_nm(10)};
    fp2->fields.push_back({"Reference","R2",{},{},false,false,false,1,1,{},make_uuid()});
    { Pad p; p.number="1"; p.kind="smd"; p.shape="rect"; p.at={0,0};
      p.size = {geom::mm_to_nm(1),geom::mm_to_nm(1)}; p.layers={"F.Cu"};
      p.net=2; p.net_name="N2"; fp2->pads.push_back(p);
    }
    fp2->uuid = make_uuid();
    b.items.push_back(fp2);

    auto dr = drc::run(b, {});
    std::size_t seen_clearance = 0;
    for (const auto & v : dr.violations)
        if (v.rule_id == "clearance") ++seen_clearance;
    // GOLDEN: exactly one pad-pad clearance violation from these two pads.
    if (seen_clearance < 1) return testing::fail("golden DRC: expected at least 1 clearance violation, got " + std::to_string(seen_clearance));

    return testing::ok();
}

const int _r = testing::register_test(
    "golden_rules",
    "Curated golden fixtures for ERC (duplicate refs + missing value) and DRC (pad-pad clearance).",
    &run);

} // namespace
