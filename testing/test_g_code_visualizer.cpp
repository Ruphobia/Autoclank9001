#include "test_runner.hpp"
#include "../modules/945_g_code_visualizer/g_code_visualizer.hpp"

namespace {

testing::TestOutcome run_g_code_visualizer() {
    g_code_visualizer::init();
    auto s = g_code_visualizer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    g_code_visualizer::shutdown();
    return testing::ok();
}

const int _reg_g_code_visualizer = testing::register_test(
    "g_code_visualizer",
    "945_g_code_visualizer: stub status check",
    &run_g_code_visualizer);

}
