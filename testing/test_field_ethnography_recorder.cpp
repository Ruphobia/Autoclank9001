// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/749_field_ethnography_recorder/field_ethnography_recorder.hpp"

namespace {

testing::TestOutcome run_field_ethnography_recorder() {
    field_ethnography_recorder::init();
    auto s = field_ethnography_recorder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    field_ethnography_recorder::shutdown();
    return testing::ok();
}

const int _reg_field_ethnography_recorder = testing::register_test(
    "field_ethnography_recorder",
    "749_field_ethnography_recorder: stub status check",
    &run_field_ethnography_recorder);

}
