#include "test_runner.hpp"
#include "../modules/345_tomography_suite/tomography_suite.hpp"

namespace {

testing::TestOutcome run_tomography_suite() {
    tomography_suite::init();
    auto s = tomography_suite::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    tomography_suite::shutdown();
    return testing::ok();
}

const int _reg_tomography_suite = testing::register_test(
    "tomography_suite",
    "345_tomography_suite: stub status check",
    &run_tomography_suite);

}
