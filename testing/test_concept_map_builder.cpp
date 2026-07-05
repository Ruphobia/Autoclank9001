// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/520_concept_map_builder/concept_map_builder.hpp"

namespace {

testing::TestOutcome run_concept_map_builder() {
    concept_map_builder::init();
    auto s = concept_map_builder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    concept_map_builder::shutdown();
    return testing::ok();
}

const int _reg_concept_map_builder = testing::register_test(
    "concept_map_builder",
    "520_concept_map_builder: stub status check",
    &run_concept_map_builder);

}
