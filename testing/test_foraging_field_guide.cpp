// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2121_foraging_field_guide/foraging_field_guide.hpp"

namespace {

testing::TestOutcome run_foraging_field_guide() {
    foraging_field_guide::init();
    auto s = foraging_field_guide::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    foraging_field_guide::shutdown();
    return testing::ok();
}

const int _reg_foraging_field_guide = testing::register_test(
    "foraging_field_guide",
    "2121_foraging_field_guide: stub status check",
    &run_foraging_field_guide);

}
