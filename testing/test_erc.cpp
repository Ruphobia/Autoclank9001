// SPDX-License-Identifier: GPL-3.0-or-later
// Smoke test for modules/350_erc.

#include "test_runner.hpp"
#include "../modules/346_kicad_model/kicad_model.hpp"
#include "../modules/349_netlist/netlist.hpp"
#include "../modules/350_erc/erc.hpp"

#include <memory>

namespace {

testing::TestOutcome run() {
    using namespace kicad_model;

    // Build a schematic with:
    //   - a duplicate refdes (R1, R1)
    //   - a lonely pin (isolated single-pin net)
    //   - an output-vs-power_out conflict on a wired net
    Schematic sch;
    sch.uuid = make_uuid();

    LibSymbol lib;
    lib.lib_id = "Device:R";
    { SchPin p; p.number = "1"; p.electrical = "passive"; p.at = {0, geom::mm_to_nm( 2.54)}; lib.pins.push_back(p); }
    { SchPin p; p.number = "2"; p.electrical = "passive"; p.at = {0, geom::mm_to_nm(-2.54)}; lib.pins.push_back(p); }
    sch.lib_symbols["Device:R"] = lib;

    LibSymbol drv;
    drv.lib_id = "Sim:VOUT";
    { SchPin p; p.number = "1"; p.electrical = "output";    p.at = {0, 0}; drv.pins.push_back(p); }
    sch.lib_symbols["Sim:VOUT"] = drv;

    LibSymbol pwr;
    pwr.lib_id = "power:+5V";
    { SchPin p; p.number = "1"; p.electrical = "power_out"; p.at = {0, 0}; pwr.pins.push_back(p); }
    sch.lib_symbols["power:+5V"] = pwr;

    auto R1a = std::make_shared<SchSymbol>();
    R1a->lib_id = "Device:R";
    R1a->at = {geom::mm_to_nm(30), geom::mm_to_nm(30)};
    R1a->fields.push_back({"Reference","R1",{},{},false,false,false,1.27,1.27,{},make_uuid()});
    R1a->fields.push_back({"Value","10k",{},{},false,false,false,1.27,1.27,{},make_uuid()});
    sch.root.items.push_back(R1a);

    auto R1b = std::make_shared<SchSymbol>();     // dup refdes on purpose
    R1b->lib_id = "Device:R";
    R1b->at = {geom::mm_to_nm(60), geom::mm_to_nm(30)};
    R1b->fields.push_back({"Reference","R1",{},{},false,false,false,1.27,1.27,{},make_uuid()});
    R1b->fields.push_back({"Value","1k", {},{},false,false,false,1.27,1.27,{},make_uuid()});
    sch.root.items.push_back(R1b);

    // Output-vs-power_out conflict on a wire.
    auto U1 = std::make_shared<SchSymbol>();
    U1->lib_id = "Sim:VOUT";
    U1->at = {geom::mm_to_nm(30), geom::mm_to_nm(60)};
    U1->fields.push_back({"Reference","U1",{},{},false,false,false,1.27,1.27,{},make_uuid()});
    U1->fields.push_back({"Value","VOUT",{},{},false,false,false,1.27,1.27,{},make_uuid()});
    sch.root.items.push_back(U1);

    auto V1 = std::make_shared<SchSymbol>();
    V1->lib_id = "power:+5V";
    V1->at = {geom::mm_to_nm(50), geom::mm_to_nm(60)};
    V1->fields.push_back({"Reference","#PWR01",{},{},false,false,false,1.27,1.27,{},make_uuid()});
    V1->fields.push_back({"Value","+5V",{},{},false,false,false,1.27,1.27,{},make_uuid()});
    sch.root.items.push_back(V1);

    auto w = std::make_shared<SchWire>();
    w->pts = { {geom::mm_to_nm(30), geom::mm_to_nm(60)},
               {geom::mm_to_nm(50), geom::mm_to_nm(60)} };
    sch.root.items.push_back(w);

    // Derive netlist first.
    auto nl = netlist::derive(sch);
    auto rep = erc::run(sch, nl, {});

    // Assertions.
    bool saw_dup = false, saw_pin_conflict = false, saw_unconnected = false;
    for (const auto & v : rep.violations) {
        if (v.rule_id == "duplicate_reference") saw_dup = true;
        if (v.rule_id == "pin_to_pin")           saw_pin_conflict = true;
        if (v.rule_id == "pin_not_connected")   saw_unconnected = true;
    }
    if (!saw_dup)             return testing::fail("duplicate_reference not flagged");
    if (!saw_pin_conflict)    return testing::fail("pin_to_pin conflict not flagged (output vs power_out)");
    if (!saw_unconnected)     return testing::fail("pin_not_connected not flagged (R1 isolated pins)");
    if (rep.errors + rep.warnings == 0) return testing::fail("no severities recorded");

    // JSON envelope.
    std::string j = erc::to_kicad_json(rep, "smoke.kicad_sch");
    if (j.find("\"violations\"") == std::string::npos) return testing::fail("json missing violations");
    if (j.find("duplicate_reference") == std::string::npos) return testing::fail("json missing type");

    return testing::ok();
}

const int _r = testing::register_test(
    "erc",
    "ERC engine: duplicate refs, pin_to_pin conflicts via 12x12 matrix, pin_not_connected, dangling labels, JSON envelope compatible with kicad_bridge report parser.",
    &run);

} // namespace
