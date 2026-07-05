// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1278_inverter_efficiency_mapper/inverter_efficiency_mapper.hpp"

namespace {

testing::TestOutcome run_inverter_efficiency_mapper() {
    inverter_efficiency_mapper::init();
    auto s = inverter_efficiency_mapper::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    inverter_efficiency_mapper::shutdown();
    return testing::ok();
}

const int _reg_inverter_efficiency_mapper = testing::register_test(
    "inverter_efficiency_mapper",
    "1278_inverter_efficiency_mapper: stub status check",
    &run_inverter_efficiency_mapper);

}
