// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1380_lab_protocol_and_eln/lab_protocol_and_eln.hpp"

namespace {

testing::TestOutcome run_lab_protocol_and_eln() {
    lab_protocol_and_eln::init();
    auto s = lab_protocol_and_eln::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    lab_protocol_and_eln::shutdown();
    return testing::ok();
}

const int _reg_lab_protocol_and_eln = testing::register_test(
    "lab_protocol_and_eln",
    "1380_lab_protocol_and_eln: stub status check",
    &run_lab_protocol_and_eln);

}
