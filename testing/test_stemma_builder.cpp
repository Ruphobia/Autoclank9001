// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/754_stemma_builder/stemma_builder.hpp"

namespace {

testing::TestOutcome run_stemma_builder() {
    stemma_builder::init();
    auto s = stemma_builder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    stemma_builder::shutdown();
    return testing::ok();
}

const int _reg_stemma_builder = testing::register_test(
    "stemma_builder",
    "754_stemma_builder: stub status check",
    &run_stemma_builder);

}
