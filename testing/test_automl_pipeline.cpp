// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/665_automl_pipeline/automl_pipeline.hpp"

namespace {

testing::TestOutcome run_automl_pipeline() {
    automl_pipeline::init();
    auto s = automl_pipeline::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    automl_pipeline::shutdown();
    return testing::ok();
}

const int _reg_automl_pipeline = testing::register_test(
    "automl_pipeline",
    "665_automl_pipeline: stub status check",
    &run_automl_pipeline);

}
