#include "test_runner.hpp"
#include "../modules/1791_gravity_log_curve_fitter/gravity_log_curve_fitter.hpp"

namespace {

testing::TestOutcome run_gravity_log_curve_fitter() {
    gravity_log_curve_fitter::init();
    auto s = gravity_log_curve_fitter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    gravity_log_curve_fitter::shutdown();
    return testing::ok();
}

const int _reg_gravity_log_curve_fitter = testing::register_test(
    "gravity_log_curve_fitter",
    "1791_gravity_log_curve_fitter: stub status check",
    &run_gravity_log_curve_fitter);

}
