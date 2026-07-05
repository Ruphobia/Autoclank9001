#include "test_runner.hpp"
#include "../modules/1615_sun_and_shade_analyzer/sun_and_shade_analyzer.hpp"

namespace {

testing::TestOutcome run_sun_and_shade_analyzer() {
    sun_and_shade_analyzer::init();
    auto s = sun_and_shade_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    sun_and_shade_analyzer::shutdown();
    return testing::ok();
}

const int _reg_sun_and_shade_analyzer = testing::register_test(
    "sun_and_shade_analyzer",
    "1615_sun_and_shade_analyzer: stub status check",
    &run_sun_and_shade_analyzer);

}
