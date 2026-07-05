// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/452_workflow_designer/workflow_designer.hpp"

namespace {

testing::TestOutcome run_workflow_designer() {
    workflow_designer::init();
    auto s = workflow_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    workflow_designer::shutdown();
    return testing::ok();
}

const int _reg_workflow_designer = testing::register_test(
    "workflow_designer",
    "452_workflow_designer: stub status check",
    &run_workflow_designer);

}
