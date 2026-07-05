// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/279_package_manager_frontend/package_manager_frontend.hpp"

namespace {

testing::TestOutcome run_package_manager_frontend() {
    package_manager_frontend::init();
    auto s = package_manager_frontend::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    package_manager_frontend::shutdown();
    return testing::ok();
}

const int _reg_package_manager_frontend = testing::register_test(
    "package_manager_frontend",
    "279_package_manager_frontend: stub status check",
    &run_package_manager_frontend);

}
