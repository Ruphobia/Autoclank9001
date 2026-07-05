#include "test_runner.hpp"
#include "../modules/1272_sun_path_shading_analyzer/sun_path_shading_analyzer.hpp"

namespace {

testing::TestOutcome run_sun_path_shading_analyzer() {
    sun_path_shading_analyzer::init();
    auto s = sun_path_shading_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    sun_path_shading_analyzer::shutdown();
    return testing::ok();
}

const int _reg_sun_path_shading_analyzer = testing::register_test(
    "sun_path_shading_analyzer",
    "1272_sun_path_shading_analyzer: stub status check",
    &run_sun_path_shading_analyzer);

}
