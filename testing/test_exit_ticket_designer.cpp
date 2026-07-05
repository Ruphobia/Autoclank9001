// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/530_exit_ticket_designer/exit_ticket_designer.hpp"

namespace {

testing::TestOutcome run_exit_ticket_designer() {
    exit_ticket_designer::init();
    auto s = exit_ticket_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    exit_ticket_designer::shutdown();
    return testing::ok();
}

const int _reg_exit_ticket_designer = testing::register_test(
    "exit_ticket_designer",
    "530_exit_ticket_designer: stub status check",
    &run_exit_ticket_designer);

}
