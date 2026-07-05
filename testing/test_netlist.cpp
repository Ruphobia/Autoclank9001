// SPDX-License-Identifier: GPL-3.0-or-later
// Smoke test for modules/349_netlist.

#include "test_runner.hpp"
#include "../modules/346_kicad_model/kicad_model.hpp"
#include "../modules/349_netlist/netlist.hpp"

#include <memory>

namespace {

testing::TestOutcome run() {
    using namespace kicad_model;

    // Build a tiny schematic by hand:
    //   R1 (pin 1)  --wire--  (pin 1) C1
    //   R1 (pin 2)  --wire--  +5V label
    //   C1 (pin 2)  --wire--  GND label
    Schematic sch;
    sch.uuid = make_uuid();

    LibSymbol libR;
    libR.lib_id = "Device:R";
    { SchPin p; p.number = "1"; p.electrical = "passive"; p.at = {0, geom::mm_to_nm( 2.54)}; libR.pins.push_back(p); }
    { SchPin p; p.number = "2"; p.electrical = "passive"; p.at = {0, geom::mm_to_nm(-2.54)}; libR.pins.push_back(p); }
    sch.lib_symbols["Device:R"] = libR;

    LibSymbol libC;
    libC.lib_id = "Device:C";
    { SchPin p; p.number = "1"; p.electrical = "passive"; p.at = {0, geom::mm_to_nm( 2.54)}; libC.pins.push_back(p); }
    { SchPin p; p.number = "2"; p.electrical = "passive"; p.at = {0, geom::mm_to_nm(-2.54)}; libC.pins.push_back(p); }
    sch.lib_symbols["Device:C"] = libC;

    auto R1 = std::make_shared<SchSymbol>();
    R1->lib_id = "Device:R";
    R1->at = {geom::mm_to_nm(50), geom::mm_to_nm(50)};
    R1->fields.push_back({"Reference","R1",{},{},false,false,false,1.27,1.27,{},make_uuid()});
    R1->fields.push_back({"Value","10k",{},{},false,false,false,1.27,1.27,{},make_uuid()});
    sch.root.items.push_back(R1);

    auto C1 = std::make_shared<SchSymbol>();
    C1->lib_id = "Device:C";
    C1->at = {geom::mm_to_nm(70), geom::mm_to_nm(50)};
    C1->fields.push_back({"Reference","C1",{},{},false,false,false,1.27,1.27,{},make_uuid()});
    C1->fields.push_back({"Value","10nF",{},{},false,false,false,1.27,1.27,{},make_uuid()});
    sch.root.items.push_back(C1);

    // Wire connecting R1.1 (at 50, 52.54) to C1.1 (at 70, 52.54)
    auto w = std::make_shared<SchWire>();
    w->pts = { {geom::mm_to_nm(50), geom::mm_to_nm(52.54)},
               {geom::mm_to_nm(70), geom::mm_to_nm(52.54)} };
    sch.root.items.push_back(w);

    // Global label "+5V" at R1.2 (at 50, 47.46)
    auto v5 = std::make_shared<SchGlobalLabel>();
    v5->text = "+5V";
    v5->at = {geom::mm_to_nm(50), geom::mm_to_nm(47.46)};
    sch.root.items.push_back(v5);

    // Global label "GND" at C1.2 (at 70, 47.46)
    auto gnd = std::make_shared<SchGlobalLabel>();
    gnd->text = "GND";
    gnd->at = {geom::mm_to_nm(70), geom::mm_to_nm(47.46)};
    sch.root.items.push_back(gnd);

    auto nl = netlist::derive(sch);
    if (nl.nets.size() < 4) return testing::fail("expected at least 4 nets (nc + 3), got " + std::to_string(nl.nets.size()));

    // Verify +5V and GND labels captured pins.
    bool saw_plus5 = false, saw_gnd = false, saw_wire = false;
    for (const auto & n : nl.nets) {
        if (n.name == "+5V") {
            saw_plus5 = true;
            bool has_r1_2 = false;
            for (const auto & p : n.pins) if (p.ref == "R1" && p.number == "2") has_r1_2 = true;
            if (!has_r1_2) return testing::fail("+5V should include R1.2");
        }
        if (n.name == "GND") {
            saw_gnd = true;
            bool has_c1_2 = false;
            for (const auto & p : n.pins) if (p.ref == "C1" && p.number == "2") has_c1_2 = true;
            if (!has_c1_2) return testing::fail("GND should include C1.2");
        }
        // The unnamed wire net should have both R1.1 and C1.1.
        bool has_r1_1 = false, has_c1_1 = false;
        for (const auto & p : n.pins) {
            if (p.ref == "R1" && p.number == "1") has_r1_1 = true;
            if (p.ref == "C1" && p.number == "1") has_c1_1 = true;
        }
        if (has_r1_1 && has_c1_1) saw_wire = true;
    }
    if (!saw_plus5) return testing::fail("no +5V net");
    if (!saw_gnd)   return testing::fail("no GND net");
    if (!saw_wire)  return testing::fail("R1.1 and C1.1 not on same wire-net");

    // KiCad netlist emit.
    std::string kn = netlist::to_kicad_netlist(nl, sch);
    if (kn.find("(export") == std::string::npos) return testing::fail("kicad netlist header");
    if (kn.find("R1")       == std::string::npos) return testing::fail("kicad netlist missing R1");
    if (kn.find("+5V")      == std::string::npos) return testing::fail("kicad netlist missing +5V");

    // SPICE netlist emit.
    std::string sp = netlist::to_spice_netlist(nl, sch, "tran 1u 1m");
    if (sp.find(".title") == std::string::npos) return testing::fail("spice title");
    if (sp.find(".tran")  == std::string::npos) return testing::fail("spice tran");
    if (sp.find(".end")   == std::string::npos) return testing::fail("spice end");
    if (sp.find("R1")     == std::string::npos) return testing::fail("spice R1 card");

    return testing::ok();
}

const int _r = testing::register_test(
    "netlist",
    "Build a mini schematic (R+C+labels), derive netlist, verify +5V/GND capture and wire join; round-trip through kicadsexpr and SPICE emitters.",
    &run);

} // namespace
