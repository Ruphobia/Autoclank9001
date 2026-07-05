// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/720_stemma_codicum_builder/stemma_codicum_builder.hpp"

namespace {

testing::TestOutcome run_stemma_codicum_builder() {
    stemma_codicum_builder::init();
    auto s = stemma_codicum_builder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    stemma_codicum_builder::shutdown();
    return testing::ok();
}

const int _reg_stemma_codicum_builder = testing::register_test(
    "stemma_codicum_builder",
    "720_stemma_codicum_builder: stub status check",
    &run_stemma_codicum_builder);

}
