#include "test_runner.hpp"
#include "../modules/507_pacing_visualizer/pacing_visualizer.hpp"

namespace {

testing::TestOutcome run_pacing_visualizer() {
    pacing_visualizer::init();
    auto s = pacing_visualizer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pacing_visualizer::shutdown();
    return testing::ok();
}

const int _reg_pacing_visualizer = testing::register_test(
    "pacing_visualizer",
    "507_pacing_visualizer: stub status check",
    &run_pacing_visualizer);

}
