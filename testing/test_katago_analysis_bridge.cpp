#include "test_runner.hpp"
#include "../modules/1968_katago_analysis_bridge/katago_analysis_bridge.hpp"

namespace {

testing::TestOutcome run_katago_analysis_bridge() {
    katago_analysis_bridge::init();
    auto s = katago_analysis_bridge::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    katago_analysis_bridge::shutdown();
    return testing::ok();
}

const int _reg_katago_analysis_bridge = testing::register_test(
    "katago_analysis_bridge",
    "1968_katago_analysis_bridge: stub status check",
    &run_katago_analysis_bridge);

}
