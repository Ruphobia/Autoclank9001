// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/747_comparative_ethics_engine/comparative_ethics_engine.hpp"

namespace {

testing::TestOutcome run_comparative_ethics_engine() {
    comparative_ethics_engine::init();
    auto s = comparative_ethics_engine::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    comparative_ethics_engine::shutdown();
    return testing::ok();
}

const int _reg_comparative_ethics_engine = testing::register_test(
    "comparative_ethics_engine",
    "747_comparative_ethics_engine: stub status check",
    &run_comparative_ethics_engine);

}
