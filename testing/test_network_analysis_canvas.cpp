#include "test_runner.hpp"
#include "../modules/1448_network_analysis_canvas/network_analysis_canvas.hpp"

namespace {

testing::TestOutcome run_network_analysis_canvas() {
    network_analysis_canvas::init();
    auto s = network_analysis_canvas::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    network_analysis_canvas::shutdown();
    return testing::ok();
}

const int _reg_network_analysis_canvas = testing::register_test(
    "network_analysis_canvas",
    "1448_network_analysis_canvas: stub status check",
    &run_network_analysis_canvas);

}
