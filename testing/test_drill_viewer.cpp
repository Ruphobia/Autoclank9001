// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/346_kicad_model/kicad_model.hpp"
#include "../modules/352_fab_writers/fab_writers.hpp"
#include "../modules/363_drill_viewer/drill_viewer.hpp"

#include <memory>

namespace {

testing::TestOutcome run() {
    using namespace kicad_model;

    Board b; b.uuid = make_uuid(); b.layers = default_2layer_stackup();
    auto fp = std::make_shared<Footprint>();
    fp->lib_id = "test:pth"; fp->at = { geom::mm_to_nm(10), geom::mm_to_nm(10) };
    fp->fields.push_back({"Reference","J1",{},{},false,false,false,1,1,{},make_uuid()});
    { Pad p; p.number="1"; p.kind="thru_hole"; p.shape="circle"; p.at={0,0};
      p.size = {geom::mm_to_nm(1.5),geom::mm_to_nm(1.5)};
      p.drill_nm = geom::mm_to_nm(0.8);
      p.layers = {"*.Cu","*.Mask"};
      fp->pads.push_back(p);
    }
    b.items.push_back(fp);
    auto v = std::make_shared<PcbVia>();
    v->at = {geom::mm_to_nm(20), geom::mm_to_nm(10)};
    v->size_nm  = geom::mm_to_nm(0.6);
    v->drill_nm = geom::mm_to_nm(0.3);
    v->net = 1;
    b.items.push_back(v);

    std::string drl = fab_writers::write_drill_pth(b, {});
    auto p = drill_viewer::parse(drl);
    if (!p.ok) return testing::fail("parse ok");
    if (p.holes.size() != 2)  return testing::fail("expected 2 holes, got " + std::to_string(p.holes.size()));

    std::string svg = drill_viewer::to_svg(p);
    if (svg.find("<svg")    == std::string::npos) return testing::fail("no svg");
    if (svg.find("<circle") == std::string::npos) return testing::fail("no circles");
    return testing::ok();
}

const int _r = testing::register_test(
    "drill_viewer",
    "Native Excellon drill parser + SVG. Round-trips via fab_writers.",
    &run);

} // namespace
