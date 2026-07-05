#include "test_runner.hpp"
#include "../modules/1140_rcs_thruster_sizer/rcs_thruster_sizer.hpp"

namespace {

testing::TestOutcome run_rcs_thruster_sizer() {
    rcs_thruster_sizer::init();
    auto s = rcs_thruster_sizer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    rcs_thruster_sizer::shutdown();
    return testing::ok();
}

const int _reg_rcs_thruster_sizer = testing::register_test(
    "rcs_thruster_sizer",
    "1140_rcs_thruster_sizer: stub status check",
    &run_rcs_thruster_sizer);

}
