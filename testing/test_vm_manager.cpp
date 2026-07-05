// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/268_vm_manager/vm_manager.hpp"

namespace {

testing::TestOutcome run_vm_manager() {
    vm_manager::init();
    auto s = vm_manager::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    vm_manager::shutdown();
    return testing::ok();
}

const int _reg_vm_manager = testing::register_test(
    "vm_manager",
    "268_vm_manager: stub status check",
    &run_vm_manager);

}
