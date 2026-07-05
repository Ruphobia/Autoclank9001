#include "test_runner.hpp"
#include "../modules/1203_gear_ratio_designer/gear_ratio_designer.hpp"

namespace {

testing::TestOutcome run_gear_ratio_designer() {
    gear_ratio_designer::init();
    auto s = gear_ratio_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    gear_ratio_designer::shutdown();
    return testing::ok();
}

const int _reg_gear_ratio_designer = testing::register_test(
    "gear_ratio_designer",
    "1203_gear_ratio_designer: stub status check",
    &run_gear_ratio_designer);

}
