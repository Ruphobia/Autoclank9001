// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/736_liturgical_calendar_engine/liturgical_calendar_engine.hpp"

namespace {

testing::TestOutcome run_liturgical_calendar_engine() {
    liturgical_calendar_engine::init();
    auto s = liturgical_calendar_engine::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    liturgical_calendar_engine::shutdown();
    return testing::ok();
}

const int _reg_liturgical_calendar_engine = testing::register_test(
    "liturgical_calendar_engine",
    "736_liturgical_calendar_engine: stub status check",
    &run_liturgical_calendar_engine);

}
