#include "test_runner.hpp"
#include "../modules/1073_scada_timeseries_viewer/scada_timeseries_viewer.hpp"

namespace {

testing::TestOutcome run_scada_timeseries_viewer() {
    scada_timeseries_viewer::init();
    auto s = scada_timeseries_viewer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    scada_timeseries_viewer::shutdown();
    return testing::ok();
}

const int _reg_scada_timeseries_viewer = testing::register_test(
    "scada_timeseries_viewer",
    "1073_scada_timeseries_viewer: stub status check",
    &run_scada_timeseries_viewer);

}
