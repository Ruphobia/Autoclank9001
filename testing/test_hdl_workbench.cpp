#include "test_runner.hpp"
#include "../modules/860_hdl_workbench/hdl_workbench.hpp"

namespace {

testing::TestOutcome run_hdl_workbench() {
    hdl_workbench::init();
    auto s = hdl_workbench::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    hdl_workbench::shutdown();
    return testing::ok();
}

const int _reg_hdl_workbench = testing::register_test(
    "hdl_workbench",
    "860_hdl_workbench: stub status check",
    &run_hdl_workbench);

}
