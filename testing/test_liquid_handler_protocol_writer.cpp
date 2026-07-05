// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1389_liquid_handler_protocol_writer/liquid_handler_protocol_writer.hpp"

namespace {

testing::TestOutcome run_liquid_handler_protocol_writer() {
    liquid_handler_protocol_writer::init();
    auto s = liquid_handler_protocol_writer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    liquid_handler_protocol_writer::shutdown();
    return testing::ok();
}

const int _reg_liquid_handler_protocol_writer = testing::register_test(
    "liquid_handler_protocol_writer",
    "1389_liquid_handler_protocol_writer: stub status check",
    &run_liquid_handler_protocol_writer);

}
