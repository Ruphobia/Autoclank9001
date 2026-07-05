// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1342_variant_annotator_and_interpreter/variant_annotator_and_interpreter.hpp"

namespace {

testing::TestOutcome run_variant_annotator_and_interpreter() {
    variant_annotator_and_interpreter::init();
    auto s = variant_annotator_and_interpreter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    variant_annotator_and_interpreter::shutdown();
    return testing::ok();
}

const int _reg_variant_annotator_and_interpreter = testing::register_test(
    "variant_annotator_and_interpreter",
    "1342_variant_annotator_and_interpreter: stub status check",
    &run_variant_annotator_and_interpreter);

}
