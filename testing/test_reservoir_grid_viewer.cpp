#include "test_runner.hpp"
#include "../modules/1082_reservoir_grid_viewer/reservoir_grid_viewer.hpp"

namespace {

testing::TestOutcome run_reservoir_grid_viewer() {
    reservoir_grid_viewer::init();
    auto s = reservoir_grid_viewer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    reservoir_grid_viewer::shutdown();
    return testing::ok();
}

const int _reg_reservoir_grid_viewer = testing::register_test(
    "reservoir_grid_viewer",
    "1082_reservoir_grid_viewer: stub status check",
    &run_reservoir_grid_viewer);

}
