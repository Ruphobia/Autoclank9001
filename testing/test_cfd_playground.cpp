// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/904_cfd_playground/cfd_playground.hpp"

namespace {

testing::TestOutcome run_cfd_playground() {
    cfd_playground::init();
    auto s = cfd_playground::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    cfd_playground::shutdown();
    return testing::ok();
}

const int _reg_cfd_playground = testing::register_test(
    "cfd_playground",
    "904_cfd_playground: stub status check",
    &run_cfd_playground);

}
