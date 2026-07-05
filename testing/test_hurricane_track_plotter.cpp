#include "test_runner.hpp"
#include "../modules/1464_hurricane_track_plotter/hurricane_track_plotter.hpp"

namespace {

testing::TestOutcome run_hurricane_track_plotter() {
    hurricane_track_plotter::init();
    auto s = hurricane_track_plotter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    hurricane_track_plotter::shutdown();
    return testing::ok();
}

const int _reg_hurricane_track_plotter = testing::register_test(
    "hurricane_track_plotter",
    "1464_hurricane_track_plotter: stub status check",
    &run_hurricane_track_plotter);

}
