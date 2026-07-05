// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/779_trickster_taxonomy_viewer/trickster_taxonomy_viewer.hpp"

namespace {

testing::TestOutcome run_trickster_taxonomy_viewer() {
    trickster_taxonomy_viewer::init();
    auto s = trickster_taxonomy_viewer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    trickster_taxonomy_viewer::shutdown();
    return testing::ok();
}

const int _reg_trickster_taxonomy_viewer = testing::register_test(
    "trickster_taxonomy_viewer",
    "779_trickster_taxonomy_viewer: stub status check",
    &run_trickster_taxonomy_viewer);

}
