// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/587_terms_of_service_builder/terms_of_service_builder.hpp"

namespace {

testing::TestOutcome run_terms_of_service_builder() {
    terms_of_service_builder::init();
    auto s = terms_of_service_builder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    terms_of_service_builder::shutdown();
    return testing::ok();
}

const int _reg_terms_of_service_builder = testing::register_test(
    "terms_of_service_builder",
    "587_terms_of_service_builder: stub status check",
    &run_terms_of_service_builder);

}
