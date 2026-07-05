#include "test_runner.hpp"
#include "../modules/1210_suspension_geometry_visualizer/suspension_geometry_visualizer.hpp"

namespace {

testing::TestOutcome run_suspension_geometry_visualizer() {
    suspension_geometry_visualizer::init();
    auto s = suspension_geometry_visualizer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    suspension_geometry_visualizer::shutdown();
    return testing::ok();
}

const int _reg_suspension_geometry_visualizer = testing::register_test(
    "suspension_geometry_visualizer",
    "1210_suspension_geometry_visualizer: stub status check",
    &run_suspension_geometry_visualizer);

}
