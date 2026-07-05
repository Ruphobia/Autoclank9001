// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/793_dialect_atlas/dialect_atlas.hpp"

namespace {

testing::TestOutcome run_dialect_atlas() {
    dialect_atlas::init();
    auto s = dialect_atlas::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    dialect_atlas::shutdown();
    return testing::ok();
}

const int _reg_dialect_atlas = testing::register_test(
    "dialect_atlas",
    "793_dialect_atlas: stub status check",
    &run_dialect_atlas);

}
