// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/346_kicad_model/kicad_model.hpp"
#include "../modules/381_pos_overrides/pos_overrides.hpp"

#include <memory>

namespace {

testing::TestOutcome run() {
    using namespace kicad_model;
    Board b; b.uuid = make_uuid(); b.layers = default_2layer_stackup();

    auto fp = std::make_shared<Footprint>();
    fp->lib_id = "test:R"; fp->at = {geom::mm_to_nm(20), geom::mm_to_nm(30)};
    fp->angle = geom::EDA_ANGLE{90.0};
    fp->placement_layer = "F.Cu";
    fp->fields.push_back({"Reference","R1",{},{},false,false,false,1,1,{},make_uuid()});
    fp->fields.push_back({"Value","10k",{},{},false,false,false,1,1,{},make_uuid()});
    fp->uuid = make_uuid();
    b.items.push_back(fp);

    pos_overrides::Table t;
    t["R1"] = { 90.0, true, 0.5, -0.3 };

    std::string csv = pos_overrides::write_pos_csv(b, t);
    if (csv.find("R1") == std::string::npos)     return testing::fail("no R1");
    if (csv.find("bottom") == std::string::npos) return testing::fail("no flip");
    if (csv.find("180") == std::string::npos && csv.find("180.0") == std::string::npos)
        return testing::fail("rotation not applied");

    std::string j = pos_overrides::to_json(t);
    auto t2 = pos_overrides::from_json(j);
    if (t2.count("R1") != 1) return testing::fail("json round-trip");
    if (!t2["R1"].flip_side) return testing::fail("json flip");
    return testing::ok();
}

const int _r = testing::register_test(
    "pos_overrides",
    "Per-part pick-and-place overrides: rotation delta, side flip, XY offset; JSON round-trip.",
    &run);

} // namespace
