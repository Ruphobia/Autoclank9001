// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/346_kicad_model/kicad_model.hpp"
#include "../modules/370_length_tuning/length_tuning.hpp"

#include <memory>

namespace {

testing::TestOutcome run() {
    using namespace kicad_model;
    Board b; b.uuid = make_uuid(); b.layers = default_2layer_stackup();
    intern_net(b, ""); intern_net(b, "CLK");

    auto t = std::make_shared<PcbTrack>();
    t->start = {geom::mm_to_nm(10), geom::mm_to_nm(10)};
    t->end   = {geom::mm_to_nm(40), geom::mm_to_nm(10)};   // 30 mm base
    t->width_nm = geom::mm_to_nm(0.2);
    t->layer = "F.Cu"; t->net = 1;
    t->uuid = make_uuid();
    b.items.push_back(t);

    length_tuning::MeanderOptions opts;
    opts.target_length_mm = 40.0;
    opts.amplitude_mm     = 1.5;
    opts.period_mm        = 3.0;

    auto ar = length_tuning::apply(b, t->uuid, opts);
    if (!ar.ok) return testing::fail("apply: " + ar.reason);
    if (ar.segments_added < 3) return testing::fail("expected multiple segments");
    if (ar.achieved_length_mm < 39.5) return testing::fail("achieved length low: " + std::to_string(ar.achieved_length_mm));

    // No-op when target <= base.
    Board b2; b2.uuid = make_uuid();
    auto t2 = std::make_shared<PcbTrack>();
    t2->start = {0, 0}; t2->end = {geom::mm_to_nm(10), 0};
    t2->width_nm = geom::mm_to_nm(0.2); t2->layer = "F.Cu"; t2->net = 1;
    t2->uuid = make_uuid();
    b2.items.push_back(t2);
    auto meander = length_tuning::compute(t2->start, t2->end, { 5.0, 1.5, 3.0 });
    if (meander.path.size() != 2) return testing::fail("no-op should be 2 points");

    return testing::ok();
}

const int _r = testing::register_test(
    "length_tuning",
    "Meander insertion hits target length; below-target target is a no-op straight line.",
    &run);

} // namespace
