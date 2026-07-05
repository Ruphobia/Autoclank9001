#include "test_runner.hpp"
#include "../modules/1195_gnu_radio_flowgraph_runner/gnu_radio_flowgraph_runner.hpp"

namespace {

testing::TestOutcome run_gnu_radio_flowgraph_runner() {
    gnu_radio_flowgraph_runner::init();
    auto s = gnu_radio_flowgraph_runner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    gnu_radio_flowgraph_runner::shutdown();
    return testing::ok();
}

const int _reg_gnu_radio_flowgraph_runner = testing::register_test(
    "gnu_radio_flowgraph_runner",
    "1195_gnu_radio_flowgraph_runner: stub status check",
    &run_gnu_radio_flowgraph_runner);

}
