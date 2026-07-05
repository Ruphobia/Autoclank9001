// SPDX-License-Identifier: GPL-3.0-or-later
// Smoke test for modules/352_fab_writers.

#include "test_runner.hpp"
#include "../modules/346_kicad_model/kicad_model.hpp"
#include "../modules/352_fab_writers/fab_writers.hpp"

#include <memory>

namespace {

testing::TestOutcome run() {
    using namespace kicad_model;

    Board b;
    b.uuid   = make_uuid();
    b.layers = default_2layer_stackup();

    // One SMD pad + one THT pad + one via + one track + edge cut.
    auto fp = std::make_shared<Footprint>();
    fp->lib_id = "test:mixed";
    fp->at     = {geom::mm_to_nm(20), geom::mm_to_nm(20)};
    fp->fields.push_back({"Reference","U1",{},{},false,false,false,1,1,{},make_uuid()});
    { Pad p;
      p.number = "1"; p.kind = "smd"; p.shape = "roundrect";
      p.at     = {0, 0};
      p.size   = {geom::mm_to_nm(1.0), geom::mm_to_nm(0.8)};
      p.layers = {"F.Cu","F.Mask","F.Paste"};
      p.net    = 1; p.net_name = "+5V";
      fp->pads.push_back(p);
    }
    { Pad p;
      p.number = "2"; p.kind = "thru_hole"; p.shape = "circle";
      p.at     = {geom::mm_to_nm(2.54), 0};
      p.size   = {geom::mm_to_nm(1.7), geom::mm_to_nm(1.7)};
      p.drill_nm = geom::mm_to_nm(0.8);
      p.layers = {"*.Cu","*.Mask"};
      p.net    = 2; p.net_name = "GND";
      fp->pads.push_back(p);
    }
    b.items.push_back(fp);

    auto v = std::make_shared<PcbVia>();
    v->at = {geom::mm_to_nm(25), geom::mm_to_nm(20)};
    v->size_nm  = geom::mm_to_nm(0.6);
    v->drill_nm = geom::mm_to_nm(0.3);
    v->net = 1;
    b.items.push_back(v);

    auto t = std::make_shared<PcbTrack>();
    t->start = {geom::mm_to_nm(20), geom::mm_to_nm(20)};
    t->end   = {geom::mm_to_nm(25), geom::mm_to_nm(20)};
    t->width_nm = geom::mm_to_nm(0.25);
    t->layer = "F.Cu";
    t->net = 1;
    b.items.push_back(t);

    auto e = std::make_shared<GrLine>();
    e->start = {0, 0};
    e->end   = {geom::mm_to_nm(50), 0};
    e->layer = "Edge.Cuts";
    b.items.push_back(e);

    // Gerber F.Cu.
    std::string g = fab_writers::write_gerber_layer(b, "F.Cu");
    if (g.find("%MOMM") == std::string::npos) return testing::fail("gerber mm mode");
    if (g.find("%FSLA") == std::string::npos) return testing::fail("gerber format spec");
    if (g.find("D01")   == std::string::npos) return testing::fail("gerber D01 (draw)");
    if (g.find("D03")   == std::string::npos) return testing::fail("gerber D03 (flash) for pad");
    if (g.find("M02")   == std::string::npos) return testing::fail("gerber M02 (end)");

    // Drill PTH.
    std::string d = fab_writers::write_drill_pth(b, {});
    if (d.find("M48")    == std::string::npos) return testing::fail("drill M48 header");
    if (d.find("METRIC") == std::string::npos) return testing::fail("drill METRIC");
    if (d.find("T1")     == std::string::npos) return testing::fail("drill T1 tool");
    if (d.find("M30")    == std::string::npos) return testing::fail("drill M30 end");

    // Pos CSV.
    std::string p = fab_writers::write_pos_csv(b);
    if (p.find("U1") == std::string::npos) return testing::fail("pos csv missing U1");
    if (p.find("Ref,") == std::string::npos) return testing::fail("pos csv header");

    // Job file.
    std::string j = fab_writers::write_job_file(b, {{"Copper,L1,Top","board-F_Cu.gbr"}}, "board.drl");
    if (j.find("FilesAttributes") == std::string::npos) return testing::fail("job file attrs");

    return testing::ok();
}

const int _r = testing::register_test(
    "fab_writers",
    "Native gerber (RS-274X + X2), Excellon drill, pick-and-place CSV, and .gbrjob writers.",
    &run);

} // namespace
