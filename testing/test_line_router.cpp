// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/346_kicad_model/kicad_model.hpp"
#include "../modules/367_line_router/line_router.hpp"

#include <memory>

namespace {

testing::TestOutcome run() {
    using namespace kicad_model;
    Board b; b.uuid = make_uuid(); b.layers = default_2layer_stackup();
    intern_net(b, ""); intern_net(b, "N1");

    // Two pads on N1 with clear line-of-sight.
    auto fp1 = std::make_shared<Footprint>();
    fp1->lib_id = "test:R"; fp1->at = {geom::mm_to_nm(10), geom::mm_to_nm(10)};
    fp1->fields.push_back({"Reference","R1",{},{},false,false,false,1,1,{},make_uuid()});
    { Pad p; p.number="1"; p.kind="smd"; p.shape="rect"; p.at={0,0};
      p.size = {geom::mm_to_nm(1), geom::mm_to_nm(1)}; p.layers={"F.Cu"};
      p.net = 1; fp1->pads.push_back(p);
    }
    fp1->uuid = make_uuid();
    b.items.push_back(fp1);

    auto fp2 = std::make_shared<Footprint>();
    fp2->lib_id = "test:R"; fp2->at = {geom::mm_to_nm(30), geom::mm_to_nm(20)};
    fp2->fields.push_back({"Reference","R2",{},{},false,false,false,1,1,{},make_uuid()});
    { Pad p; p.number="1"; p.kind="smd"; p.shape="rect"; p.at={0,0};
      p.size = {geom::mm_to_nm(1), geom::mm_to_nm(1)}; p.layers={"F.Cu"};
      p.net = 1; fp2->pads.push_back(p);
    }
    fp2->uuid = make_uuid();
    b.items.push_back(fp2);

    auto pre = b.items.size();
    auto res = line_router::route_all_unrouted(b, {});
    if (res.routed_pairs < 1) return testing::fail("expected at least one routed pair");
    if (b.items.size() <= pre) return testing::fail("no tracks added");

    // Same test but with an obstacle in the middle (on a different net).
    Board b2; b2.uuid = make_uuid(); b2.layers = default_2layer_stackup();
    intern_net(b2, ""); intern_net(b2, "N1"); intern_net(b2, "N2");
    auto fp3 = std::make_shared<Footprint>();
    fp3->lib_id = "test:R"; fp3->at = {geom::mm_to_nm(10), geom::mm_to_nm(10)};
    fp3->fields.push_back({"Reference","R1",{},{},false,false,false,1,1,{},make_uuid()});
    { Pad p; p.number="1"; p.kind="smd"; p.shape="rect"; p.at={0,0};
      p.size = {geom::mm_to_nm(1),geom::mm_to_nm(1)}; p.layers={"F.Cu"};
      p.net=1; fp3->pads.push_back(p);
    }
    fp3->uuid = make_uuid();
    b2.items.push_back(fp3);
    auto fp4 = std::make_shared<Footprint>();
    fp4->lib_id = "test:R"; fp4->at = {geom::mm_to_nm(30), geom::mm_to_nm(10)};
    fp4->fields.push_back({"Reference","R2",{},{},false,false,false,1,1,{},make_uuid()});
    { Pad p; p.number="1"; p.kind="smd"; p.shape="rect"; p.at={0,0};
      p.size = {geom::mm_to_nm(1),geom::mm_to_nm(1)}; p.layers={"F.Cu"};
      p.net=1; fp4->pads.push_back(p);
    }
    fp4->uuid = make_uuid();
    b2.items.push_back(fp4);
    // Big obstacle pad on a different net crossing both L-shape candidates.
    auto obs = std::make_shared<Footprint>();
    obs->lib_id = "test:big"; obs->at = {geom::mm_to_nm(20), geom::mm_to_nm(10)};
    obs->fields.push_back({"Reference","U1",{},{},false,false,false,1,1,{},make_uuid()});
    { Pad p; p.number="1"; p.kind="smd"; p.shape="rect"; p.at={0,0};
      p.size = {geom::mm_to_nm(20),geom::mm_to_nm(20)}; p.layers={"F.Cu"};
      p.net=2; obs->pads.push_back(p);
    }
    obs->uuid = make_uuid();
    b2.items.push_back(obs);

    auto res2 = line_router::route_all_unrouted(b2, {});
    if (res2.obstructed_pairs < 1) return testing::fail("expected obstruction to be reported");
    return testing::ok();
}

const int _r = testing::register_test(
    "line_router",
    "Line router: clear straight-shot routes commit tracks; obstructed pairs reported.",
    &run);

} // namespace
