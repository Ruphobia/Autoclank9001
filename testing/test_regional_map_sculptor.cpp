#include "test_runner.hpp"
#include "../modules/1953_regional_map_sculptor/regional_map_sculptor.hpp"

namespace {

testing::TestOutcome run_regional_map_sculptor() {
    regional_map_sculptor::init();
    auto s = regional_map_sculptor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    regional_map_sculptor::shutdown();
    return testing::ok();
}

const int _reg_regional_map_sculptor = testing::register_test(
    "regional_map_sculptor",
    "1953_regional_map_sculptor: stub status check",
    &run_regional_map_sculptor);

}
