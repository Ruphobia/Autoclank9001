#include "test_runner.hpp"
#include "../modules/900_linear_algebra_studio/linear_algebra_studio.hpp"

namespace {

testing::TestOutcome run_linear_algebra_studio() {
    linear_algebra_studio::init();
    auto s = linear_algebra_studio::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    linear_algebra_studio::shutdown();
    return testing::ok();
}

const int _reg_linear_algebra_studio = testing::register_test(
    "linear_algebra_studio",
    "900_linear_algebra_studio: stub status check",
    &run_linear_algebra_studio);

}
