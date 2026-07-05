#include "test_runner.hpp"
#include "../modules/1338_sanger_trace_viewer/sanger_trace_viewer.hpp"

namespace {

testing::TestOutcome run_sanger_trace_viewer() {
    sanger_trace_viewer::init();
    auto s = sanger_trace_viewer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    sanger_trace_viewer::shutdown();
    return testing::ok();
}

const int _reg_sanger_trace_viewer = testing::register_test(
    "sanger_trace_viewer",
    "1338_sanger_trace_viewer: stub status check",
    &run_sanger_trace_viewer);

}
