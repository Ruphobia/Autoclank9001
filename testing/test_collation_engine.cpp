// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/733_collation_engine/collation_engine.hpp"

namespace {

testing::TestOutcome run_collation_engine() {
    collation_engine::init();
    auto s = collation_engine::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    collation_engine::shutdown();
    return testing::ok();
}

const int _reg_collation_engine = testing::register_test(
    "collation_engine",
    "733_collation_engine: stub status check",
    &run_collation_engine);

}
