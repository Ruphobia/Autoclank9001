#include "test_runner.hpp"
#include "../modules/1651_color_grading/color_grading.hpp"

namespace {

testing::TestOutcome run_color_grading() {
    color_grading::init();
    auto s = color_grading::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    color_grading::shutdown();
    return testing::ok();
}

const int _reg_color_grading = testing::register_test(
    "color_grading",
    "1651_color_grading: stub status check",
    &run_color_grading);

}
