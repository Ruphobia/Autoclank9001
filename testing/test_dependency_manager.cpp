// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/094_dependency_manager/dependency_manager.hpp"

namespace {

testing::TestOutcome run_dependency_manager() {
    dependency_manager::init();
    auto s = dependency_manager::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    dependency_manager::shutdown();
    return testing::ok();
}

const int _reg_dependency_manager = testing::register_test(
    "dependency_manager",
    "094_dependency_manager: stub status check",
    &run_dependency_manager);

}
