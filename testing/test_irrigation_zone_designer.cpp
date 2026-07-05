#include "test_runner.hpp"
#include "../modules/2032_irrigation_zone_designer/irrigation_zone_designer.hpp"

namespace {

testing::TestOutcome run_irrigation_zone_designer() {
    irrigation_zone_designer::init();
    auto s = irrigation_zone_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    irrigation_zone_designer::shutdown();
    return testing::ok();
}

const int _reg_irrigation_zone_designer = testing::register_test(
    "irrigation_zone_designer",
    "2032_irrigation_zone_designer: stub status check",
    &run_irrigation_zone_designer);

}
