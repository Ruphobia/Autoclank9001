#include "test_runner.hpp"
#include "../modules/1807_hot_sauce_capsaicin_calculator/hot_sauce_capsaicin_calculator.hpp"

namespace {

testing::TestOutcome run_hot_sauce_capsaicin_calculator() {
    hot_sauce_capsaicin_calculator::init();
    auto s = hot_sauce_capsaicin_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    hot_sauce_capsaicin_calculator::shutdown();
    return testing::ok();
}

const int _reg_hot_sauce_capsaicin_calculator = testing::register_test(
    "hot_sauce_capsaicin_calculator",
    "1807_hot_sauce_capsaicin_calculator: stub status check",
    &run_hot_sauce_capsaicin_calculator);

}
