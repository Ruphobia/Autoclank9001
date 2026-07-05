// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/188_bus_protocol_sniffer/bus_protocol_sniffer.hpp"

namespace {

testing::TestOutcome run_bus_protocol_sniffer() {
    bus_protocol_sniffer::init();
    auto s = bus_protocol_sniffer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    bus_protocol_sniffer::shutdown();
    return testing::ok();
}

const int _reg_bus_protocol_sniffer = testing::register_test(
    "bus_protocol_sniffer",
    "188_bus_protocol_sniffer: stub status check",
    &run_bus_protocol_sniffer);

}
