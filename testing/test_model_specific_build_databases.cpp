// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2198_model_specific_build_databases/model_specific_build_databases.hpp"

namespace {

testing::TestOutcome run_model_specific_build_databases() {
    model_specific_build_databases::init();
    auto s = model_specific_build_databases::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    model_specific_build_databases::shutdown();
    return testing::ok();
}

const int _reg_model_specific_build_databases = testing::register_test(
    "model_specific_build_databases",
    "2198_model_specific_build_databases: stub status check",
    &run_model_specific_build_databases);

}
