// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/346_kicad_model/kicad_model.hpp"
#include "../modules/352_fab_writers/fab_writers.hpp"
#include "../modules/358_gerber_viewer_native/gerber_viewer_native.hpp"

#include <memory>

namespace {

testing::TestOutcome run() {
    using namespace kicad_model;

    // Build a tiny board and emit a gerber, then re-parse it.
    Board b; b.uuid = make_uuid(); b.layers = default_2layer_stackup();
    auto fp = std::make_shared<Footprint>();
    fp->lib_id = "test:R"; fp->at = {geom::mm_to_nm(10), geom::mm_to_nm(10)};
    fp->fields.push_back({"Reference","R1",{},{},false,false,false,1,1,{},make_uuid()});
    { Pad p; p.number="1"; p.kind="smd"; p.shape="rect"; p.at={0,0}; p.size={geom::mm_to_nm(1),geom::mm_to_nm(1)}; p.layers={"F.Cu"}; p.net=1; fp->pads.push_back(p); }
    b.items.push_back(fp);
    auto t = std::make_shared<PcbTrack>();
    t->start = {geom::mm_to_nm(10), geom::mm_to_nm(10)};
    t->end   = {geom::mm_to_nm(20), geom::mm_to_nm(10)};
    t->width_nm = geom::mm_to_nm(0.2); t->layer = "F.Cu"; t->net = 1;
    b.items.push_back(t);

    std::string g = fab_writers::write_gerber_layer(b, "F.Cu");
    auto p = gerber_viewer_native::parse(g);
    if (!p.ok)              return testing::fail("parse ok");
    if (p.lines.empty())    return testing::fail("no line ops from track");
    if (p.flashes.empty())  return testing::fail("no flash op from pad");
    if (!p.bbox.valid())    return testing::fail("empty bbox");

    std::string svg = gerber_viewer_native::to_svg(p, {});
    if (svg.find("<svg")     == std::string::npos) return testing::fail("no svg root");
    if (svg.find("<line")    == std::string::npos) return testing::fail("no line output");
    if (svg.find("<circle")  == std::string::npos) return testing::fail("no circle output");

    return testing::ok();
}

const int _r = testing::register_test(
    "gerber_viewer_native",
    "Parse a gerber emitted by our own writer, verify line + flash ops, produce SVG.",
    &run);

} // namespace
