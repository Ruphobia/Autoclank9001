// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/080_annotation_workbench/annotation_workbench.hpp"

namespace {

testing::TestOutcome run_annotation_workbench() {
    annotation_workbench::init();
    auto s = annotation_workbench::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    annotation_workbench::shutdown();
    return testing::ok();
}

const int _reg_annotation_workbench = testing::register_test(
    "annotation_workbench",
    "080_annotation_workbench: stub status check",
    &run_annotation_workbench);

}
