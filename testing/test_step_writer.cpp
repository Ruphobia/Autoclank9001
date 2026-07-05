// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/346_kicad_model/kicad_model.hpp"
#include "../modules/372_step_writer/step_writer.hpp"

#include <memory>

namespace {

testing::TestOutcome run() {
    using namespace kicad_model;
    Board b; b.uuid = make_uuid(); b.layers = default_2layer_stackup();
    for (int i = 0; i < 4; ++i) {
        auto e = std::make_shared<GrLine>();
        auto p0x = i < 2 ? 0 : 50, p0y = i < 2 ? (i * 30) : (i == 2 ? 30 : 0);
        auto p1x = i < 2 ? 50 : 0, p1y = i < 2 ? (i * 30) : (i == 2 ? 30 : 0);
        e->start = { geom::mm_to_nm(p0x), geom::mm_to_nm(p0y) };
        e->end   = { geom::mm_to_nm(p1x), geom::mm_to_nm(p1y) };
        e->layer = "Edge.Cuts"; e->uuid = make_uuid();
        b.items.push_back(e);
    }
    auto fp = std::make_shared<Footprint>();
    fp->lib_id = "test:R";
    fp->at = { geom::mm_to_nm(20), geom::mm_to_nm(15) };
    fp->fields.push_back({"Reference","R1",{},{},false,false,false,1,1,{},make_uuid()});
    fp->uuid = make_uuid();
    b.items.push_back(fp);

    std::string s = step_writer::write(b, {});
    if (s.find("ISO-10303-21;")            == std::string::npos) return testing::fail("header");
    if (s.find("MANIFOLD_SOLID_BREP")      == std::string::npos) return testing::fail("solid");
    if (s.find("PRODUCT('board'")          == std::string::npos) return testing::fail("product");
    if (s.find("END-ISO-10303-21;")        == std::string::npos) return testing::fail("footer");
    return testing::ok();
}

const int _r = testing::register_test(
    "step_writer",
    "Minimal STEP AP214: substrate + component placeholders + product wrapper.",
    &run);

} // namespace
