// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/424_reference_manager/reference_manager.hpp"

namespace {

testing::TestOutcome run_reference_manager() {
    reference_manager::init();
    auto s = reference_manager::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    reference_manager::shutdown();
    return testing::ok();
}

const int _reg_reference_manager = testing::register_test(
    "reference_manager",
    "424_reference_manager: stub status check",
    &run_reference_manager);

}
