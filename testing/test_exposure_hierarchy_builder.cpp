// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2184_exposure_hierarchy_builder/exposure_hierarchy_builder.hpp"

namespace {

testing::TestOutcome run_exposure_hierarchy_builder() {
    exposure_hierarchy_builder::init();
    auto s = exposure_hierarchy_builder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    exposure_hierarchy_builder::shutdown();
    return testing::ok();
}

const int _reg_exposure_hierarchy_builder = testing::register_test(
    "exposure_hierarchy_builder",
    "2184_exposure_hierarchy_builder: stub status check",
    &run_exposure_hierarchy_builder);

}
