#include "test_runner.hpp"
#include "../modules/1579_map_viewer/map_viewer.hpp"

namespace {

testing::TestOutcome run_map_viewer() {
    map_viewer::init();
    auto s = map_viewer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    map_viewer::shutdown();
    return testing::ok();
}

const int _reg_map_viewer = testing::register_test(
    "map_viewer",
    "1579_map_viewer: stub status check",
    &run_map_viewer);

}
