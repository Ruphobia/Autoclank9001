// SPDX-License-Identifier: GPL-3.0-or-later
// Smoke test for modules/346_kicad_model.

#include "test_runner.hpp"
#include "../modules/346_kicad_model/kicad_model.hpp"

namespace {

testing::TestOutcome run() {
    using namespace kicad_model;

    // UUIDs unique enough for a sanity check.
    UUID a = make_uuid(), b = make_uuid();
    if (a.size() != 36 || b.size() != 36) return testing::fail("uuid length");
    if (a == b)                            return testing::fail("uuid not unique");

    // Build a tiny schematic.
    Schematic sch;
    sch.uuid = make_uuid();

    auto sym = std::make_shared<SchSymbol>();
    sym->lib_id = "Device:R";
    sym->at    = {geom::mm_to_nm(50), geom::mm_to_nm(50)};
    sym->fields.push_back({ "Reference", "R1", {}, {}, false, false, false, 1.27, 1.27, {}, make_uuid() });
    sym->fields.push_back({ "Value",     "10k", {}, {}, false, false, false, 1.27, 1.27, {}, make_uuid() });
    sch.root.items.push_back(sym);

    auto wire = std::make_shared<SchWire>();
    wire->pts = { {0,0}, {geom::mm_to_nm(20), 0} };
    sch.root.items.push_back(wire);

    if (sym->reference() != "R1") return testing::fail("field lookup");
    if (sym->value()     != "10k") return testing::fail("field lookup value");
    if (sch.root.items.size() != 2) return testing::fail("items count");

    // Down-cast via as<T>.
    if (!sch.root.items[0]->as<SchSymbol>())  return testing::fail("as SchSymbol");
    if ( sch.root.items[0]->as<SchWire>())    return testing::fail("false-positive as SchWire");
    if (!sch.root.items[1]->as<SchWire>())    return testing::fail("as SchWire");

    // Build a tiny board.
    Board board;
    board.uuid = make_uuid();
    board.layers = default_2layer_stackup();
    if (board.layers.size() < 4) return testing::fail("stackup size");

    if (intern_net(board, "")    != 0) return testing::fail("net 0");
    if (intern_net(board, "+5V") != 1) return testing::fail("net 1");
    if (intern_net(board, "GND") != 2) return testing::fail("net 2");
    if (intern_net(board, "+5V") != 1) return testing::fail("net idempotent");

    auto fp = std::make_shared<Footprint>();
    fp->lib_id = "Resistor_SMD:R_0603_1608Metric";
    fp->at     = {geom::mm_to_nm(20), geom::mm_to_nm(20)};
    Pad p;
    p.number = "1"; p.kind = "smd"; p.shape = "roundrect";
    p.at     = {-geom::mm_to_nm(0.75), 0};
    p.size   = {geom::mm_to_nm(0.9), geom::mm_to_nm(0.9)};
    p.layers = {"F.Cu","F.Mask","F.Paste"};
    p.net    = 1; p.net_name = "+5V";
    fp->pads.push_back(std::move(p));
    board.items.push_back(fp);

    auto track = std::make_shared<PcbTrack>();
    track->start = {geom::mm_to_nm(20),  geom::mm_to_nm(20)};
    track->end   = {geom::mm_to_nm(30),  geom::mm_to_nm(20)};
    track->width_nm = geom::mm_to_nm(0.2);
    track->layer = "F.Cu";
    track->net = 1;
    board.items.push_back(track);

    if (!board.items[0]->as<Footprint>()) return testing::fail("footprint downcast");
    if (!board.items[1]->as<PcbTrack>())  return testing::fail("track downcast");

    return testing::ok();
}

const int _r = testing::register_test(
    "kicad_model",
    "In-memory data model: uuids, schematic items, PCB items, field lookup, safe downcast, net interning, stackup.",
    &run);

} // namespace
