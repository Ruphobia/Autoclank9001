// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/422_contact_manager/contact_manager.hpp"

namespace {

testing::TestOutcome run_contact_manager() {
    contact_manager::init();
    auto s = contact_manager::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    contact_manager::shutdown();
    return testing::ok();
}

const int _reg_contact_manager = testing::register_test(
    "contact_manager",
    "422_contact_manager: stub status check",
    &run_contact_manager);

}
