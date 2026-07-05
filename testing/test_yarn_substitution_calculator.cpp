#include "test_runner.hpp"
#include "../modules/1777_yarn_substitution_calculator/yarn_substitution_calculator.hpp"

namespace {

testing::TestOutcome run_yarn_substitution_calculator() {
    yarn_substitution_calculator::init();
    auto s = yarn_substitution_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    yarn_substitution_calculator::shutdown();
    return testing::ok();
}

const int _reg_yarn_substitution_calculator = testing::register_test(
    "yarn_substitution_calculator",
    "1777_yarn_substitution_calculator: stub status check",
    &run_yarn_substitution_calculator);

}
