// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/965_resin_slicer/resin_slicer.hpp"

namespace {

testing::TestOutcome run_resin_slicer() {
    resin_slicer::init();
    auto s = resin_slicer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    resin_slicer::shutdown();
    return testing::ok();
}

const int _reg_resin_slicer = testing::register_test(
    "resin_slicer",
    "965_resin_slicer: stub status check",
    &run_resin_slicer);

}
