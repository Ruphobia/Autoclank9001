#include "test_runner.hpp"
#include "../modules/461_spc_control_chart_studio/spc_control_chart_studio.hpp"

namespace {

testing::TestOutcome run_spc_control_chart_studio() {
    spc_control_chart_studio::init();
    auto s = spc_control_chart_studio::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    spc_control_chart_studio::shutdown();
    return testing::ok();
}

const int _reg_spc_control_chart_studio = testing::register_test(
    "spc_control_chart_studio",
    "461_spc_control_chart_studio: stub status check",
    &run_spc_control_chart_studio);

}
