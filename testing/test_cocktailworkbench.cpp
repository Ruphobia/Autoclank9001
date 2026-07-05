// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1825_cocktailworkbench/cocktailworkbench.hpp"

namespace {

testing::TestOutcome run_cocktailworkbench() {
    cocktailworkbench::init();
    auto s = cocktailworkbench::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    cocktailworkbench::shutdown();
    return testing::ok();
}

const int _reg_cocktailworkbench = testing::register_test(
    "cocktailworkbench",
    "1825_cocktailworkbench: stub status check",
    &run_cocktailworkbench);

}
