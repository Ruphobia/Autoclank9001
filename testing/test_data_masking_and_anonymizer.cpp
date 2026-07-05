// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/244_data_masking_and_anonymizer/data_masking_and_anonymizer.hpp"

namespace {

testing::TestOutcome run_data_masking_and_anonymizer() {
    data_masking_and_anonymizer::init();
    auto s = data_masking_and_anonymizer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    data_masking_and_anonymizer::shutdown();
    return testing::ok();
}

const int _reg_data_masking_and_anonymizer = testing::register_test(
    "data_masking_and_anonymizer",
    "244_data_masking_and_anonymizer: stub status check",
    &run_data_masking_and_anonymizer);

}
