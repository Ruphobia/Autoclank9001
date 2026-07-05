#include "test_runner.hpp"
#include "../modules/1948_encounter_builder/encounter_builder.hpp"

namespace {

testing::TestOutcome run_encounter_builder() {
    encounter_builder::init();
    auto s = encounter_builder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    encounter_builder::shutdown();
    return testing::ok();
}

const int _reg_encounter_builder = testing::register_test(
    "encounter_builder",
    "1948_encounter_builder: stub status check",
    &run_encounter_builder);

}
