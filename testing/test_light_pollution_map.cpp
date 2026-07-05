#include "test_runner.hpp"
#include "../modules/1494_light_pollution_map/light_pollution_map.hpp"

namespace {

testing::TestOutcome run_light_pollution_map() {
    light_pollution_map::init();
    auto s = light_pollution_map::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    light_pollution_map::shutdown();
    return testing::ok();
}

const int _reg_light_pollution_map = testing::register_test(
    "light_pollution_map",
    "1494_light_pollution_map: stub status check",
    &run_light_pollution_map);

}
