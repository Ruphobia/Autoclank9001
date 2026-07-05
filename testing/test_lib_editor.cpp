// SPDX-License-Identifier: GPL-3.0-or-later
// Smoke test for modules/356_lib_editor.

#include "test_runner.hpp"
#include "../modules/356_lib_editor/lib_editor.hpp"

#include <string>

namespace {

testing::TestOutcome run() {
    // --- Symbol edits ---
    lib_editor::SymbolDoc s;
    s.lib.lib_id = "Test:U1";
    kicad_model::SchPin p;
    p.number = "1"; p.name = "VCC"; p.electrical = "power_in";
    p.at = { geom::mm_to_nm(0), geom::mm_to_nm(5) };
    auto id = lib_editor::add_pin(s, p);
    if (id.empty()) return testing::fail("add_pin no uuid");
    if (s.lib.pins.size() != 1 || !s.dirty) return testing::fail("add_pin did not add");
    if (!lib_editor::move_pin(s, id, 100000, 200000)) return testing::fail("move_pin");
    if (s.lib.pins[0].at.x != 100000 || s.lib.pins[0].at.y != geom::mm_to_nm(5) + 200000)
        return testing::fail("move_pin coords");
    lib_editor::set_field(s, "Reference", "U");
    if (s.lib.fields.empty() || s.lib.fields[0].name != "Reference" || s.lib.fields[0].value != "U")
        return testing::fail("set_field");
    if (!lib_editor::remove_pin(s, id)) return testing::fail("remove_pin");
    if (!s.lib.pins.empty()) return testing::fail("remove_pin did not remove");

    // --- Footprint edits ---
    lib_editor::FootprintDoc fp;
    fp.fp.lib_id = "Test:MYFP";
    kicad_model::Pad pad;
    pad.number = "1"; pad.kind = "smd"; pad.shape = "rect";
    pad.at   = { 0, 0 };
    pad.size = { geom::mm_to_nm(0.8), geom::mm_to_nm(0.8) };
    pad.layers = {"F.Cu"};
    auto pid = lib_editor::add_pad(fp, pad);
    if (pid.empty() || fp.fp.pads.size() != 1) return testing::fail("add_pad");
    lib_editor::attach_3d_model(fp, "${KIPRJMOD}/models/mine.wrl");
    if (fp.fp.raw_graphics_sexpr.empty()) return testing::fail("attach 3d");
    if (fp.fp.raw_graphics_sexpr.back().find("(model") == std::string::npos)
        return testing::fail("attach 3d shape");

    // --- Mesh summariser ---
    const char * wrl =
        "#VRML V2.0 utf8\n"
        "Shape { geometry IndexedFaceSet {\n"
        "  coord Coordinate { point [\n"
        "    0 0 0, 1 0 0, 1 1 0, 0 1 0\n"
        "  ] }\n"
        "  coordIndex [ 0 1 2 -1  0 2 3 -1 ]\n"
        "} }\n";
    auto ms = lib_editor::summarize_mesh(wrl, "wrl");
    if (ms.format != "wrl")             return testing::fail("format");
    if (ms.vertices == 0)               return testing::fail("verts");
    if (ms.faces == 0)                  return testing::fail("faces");
    if (ms.hi_x <= ms.lo_x)              return testing::fail("aabb");

    return testing::ok();
}

const int _r = testing::register_test(
    "lib_editor",
    "Symbol + footprint doc mutation, 3D-model attach, mesh summariser (WRL vertex/face + AABB).",
    &run);

} // namespace
