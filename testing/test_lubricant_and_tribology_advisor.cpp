#include "test_runner.hpp"
#include "../modules/962_lubricant_and_tribology_advisor/lubricant_and_tribology_advisor.hpp"

namespace {

testing::TestOutcome run_lubricant_and_tribology_advisor() {
    lubricant_and_tribology_advisor::init();
    auto s = lubricant_and_tribology_advisor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    lubricant_and_tribology_advisor::shutdown();
    return testing::ok();
}

const int _reg_lubricant_and_tribology_advisor = testing::register_test(
    "lubricant_and_tribology_advisor",
    "962_lubricant_and_tribology_advisor: stub status check",
    &run_lubricant_and_tribology_advisor);

}
