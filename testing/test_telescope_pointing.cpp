// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1481_telescope_pointing/telescope_pointing.hpp"

namespace {

testing::TestOutcome run_telescope_pointing() {
    telescope_pointing::init();
    auto s = telescope_pointing::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    telescope_pointing::shutdown();
    return testing::ok();
}

const int _reg_telescope_pointing = testing::register_test(
    "telescope_pointing",
    "1481_telescope_pointing: stub status check",
    &run_telescope_pointing);

}
