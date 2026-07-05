// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/346_kicad_model/kicad_model.hpp"
#include "../modules/371_diff_pair/diff_pair.hpp"

namespace {

testing::TestOutcome run() {
    using namespace kicad_model;
    Board b; b.uuid = make_uuid(); b.layers = default_2layer_stackup();
    intern_net(b, ""); intern_net(b, "USB_DP"); intern_net(b, "USB_DN");

    diff_pair::Config cfg;
    cfg.p_net = 1; cfg.n_net = 2;
    cfg.layer = "F.Cu"; cfg.track_width_nm = geom::mm_to_nm(0.2); cfg.gap_nm = geom::mm_to_nm(0.25);

    auto pre = b.items.size();
    auto r = diff_pair::route(b,
        {geom::mm_to_nm(0), geom::mm_to_nm(10)},
        {geom::mm_to_nm(30), geom::mm_to_nm(10)},
        {geom::mm_to_nm(0), geom::mm_to_nm(10.5)},
        {geom::mm_to_nm(30), geom::mm_to_nm(10.5)},
        cfg);
    if (!r.ok) return testing::fail("route: " + r.reason);
    if (r.p_segments < 1 || r.n_segments < 1)
        return testing::fail("expected segments on both nets");
    if (b.items.size() - pre < 2) return testing::fail("no tracks added");
    return testing::ok();
}

const int _r = testing::register_test(
    "diff_pair",
    "Differential-pair straight-run helper; both nets get parallel tracks with constant gap.",
    &run);

} // namespace
