// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1245_repair_procedure_torque_sequence_viewer/repair_procedure_torque_sequence_viewer.hpp"

namespace {

testing::TestOutcome run_repair_procedure_torque_sequence_viewer() {
    repair_procedure_torque_sequence_viewer::init();
    auto s = repair_procedure_torque_sequence_viewer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    repair_procedure_torque_sequence_viewer::shutdown();
    return testing::ok();
}

const int _reg_repair_procedure_torque_sequence_viewer = testing::register_test(
    "repair_procedure_torque_sequence_viewer",
    "1245_repair_procedure_torque_sequence_viewer: stub status check",
    &run_repair_procedure_torque_sequence_viewer);

}
