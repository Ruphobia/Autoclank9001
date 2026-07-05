// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1387_read_aligner_and_variant_caller/read_aligner_and_variant_caller.hpp"

namespace {

testing::TestOutcome run_read_aligner_and_variant_caller() {
    read_aligner_and_variant_caller::init();
    auto s = read_aligner_and_variant_caller::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    read_aligner_and_variant_caller::shutdown();
    return testing::ok();
}

const int _reg_read_aligner_and_variant_caller = testing::register_test(
    "read_aligner_and_variant_caller",
    "1387_read_aligner_and_variant_caller: stub status check",
    &run_read_aligner_and_variant_caller);

}
