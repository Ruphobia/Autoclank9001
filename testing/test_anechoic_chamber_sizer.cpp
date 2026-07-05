// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1012_anechoic_chamber_sizer/anechoic_chamber_sizer.hpp"

namespace {

testing::TestOutcome run_anechoic_chamber_sizer() {
    anechoic_chamber_sizer::init();
    auto s = anechoic_chamber_sizer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    anechoic_chamber_sizer::shutdown();
    return testing::ok();
}

const int _reg_anechoic_chamber_sizer = testing::register_test(
    "anechoic_chamber_sizer",
    "1012_anechoic_chamber_sizer: stub status check",
    &run_anechoic_chamber_sizer);

}
