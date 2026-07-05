#include "test_runner.hpp"
#include "../modules/1160_flow_visualizer/flow_visualizer.hpp"

namespace {

testing::TestOutcome run_flow_visualizer() {
    flow_visualizer::init();
    auto s = flow_visualizer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    flow_visualizer::shutdown();
    return testing::ok();
}

const int _reg_flow_visualizer = testing::register_test(
    "flow_visualizer",
    "1160_flow_visualizer: stub status check",
    &run_flow_visualizer);

}
