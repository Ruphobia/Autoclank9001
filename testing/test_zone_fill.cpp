// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/346_kicad_model/kicad_model.hpp"
#include "../modules/360_zone_fill/zone_fill.hpp"

#include <memory>

namespace {

testing::TestOutcome run() {
    using namespace kicad_model;
    Board b; b.uuid = make_uuid(); b.layers = default_2layer_stackup();
    intern_net(b, ""); intern_net(b, "GND"); intern_net(b, "SIGNAL");

    // A GND zone on F.Cu covering a 20x20 mm area.
    auto z = std::make_shared<Zone>();
    z->net = 1; z->net_name = "GND";
    z->layers = {"F.Cu"};
    z->clearance_nm = geom::mm_to_nm(0.2);
    geom::SHAPE_LINE_CHAIN outline;
    outline.append({0,0});
    outline.append({geom::mm_to_nm(20), 0});
    outline.append({geom::mm_to_nm(20), geom::mm_to_nm(20)});
    outline.append({0, geom::mm_to_nm(20)});
    outline.set_closed(true);
    z->polys.push_back(std::move(outline));
    z->uuid = make_uuid();
    b.items.push_back(z);

    // Pad on SIGNAL net inside the zone (obstacle).
    auto fp = std::make_shared<Footprint>();
    fp->lib_id = "test:R"; fp->at = {geom::mm_to_nm(10), geom::mm_to_nm(10)};
    fp->fields.push_back({"Reference","R1",{},{},false,false,false,1,1,{},make_uuid()});
    { Pad p; p.number="1"; p.kind="smd"; p.shape="rect"; p.at={0,0};
      p.size = {geom::mm_to_nm(1), geom::mm_to_nm(1)}; p.layers={"F.Cu"};
      p.net = 2; p.net_name = "SIGNAL"; fp->pads.push_back(p);
    }
    fp->uuid = make_uuid();
    b.items.push_back(fp);

    // Pad on GND net (should be left alone under MVP thermal policy).
    auto fp2 = std::make_shared<Footprint>();
    fp2->lib_id = "test:R"; fp2->at = {geom::mm_to_nm(15), geom::mm_to_nm(10)};
    { Pad p; p.number="1"; p.kind="smd"; p.shape="rect"; p.at={0,0};
      p.size = {geom::mm_to_nm(1), geom::mm_to_nm(1)}; p.layers={"F.Cu"};
      p.net = 1; fp2->pads.push_back(p);
    }
    fp2->uuid = make_uuid();
    b.items.push_back(fp2);

    auto r = zone_fill::fill_all(b, {});
    if (r.zones_processed != 1)  return testing::fail("zones_processed");
    if (r.obstacles_carved != 1) return testing::fail("expected 1 obstacle (SIGNAL pad)");

    // Now the zone should have a filled_polys with a hole around SIGNAL pad.
    auto * zone = static_cast<Zone*>(b.items[0].get());
    if (zone->filled_polys.empty()) return testing::fail("no filled_polys");
    const auto & poly = zone->filled_polys[0].polygon(0);
    if (poly.holes.size() != 1) return testing::fail("expected 1 hole");

    return testing::ok();
}

const int _r = testing::register_test(
    "zone_fill",
    "Zone fill: outline preserved, different-net pads carved as rectangular holes, same-net pads left alone.",
    &run);

} // namespace
