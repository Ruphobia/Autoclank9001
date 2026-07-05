#include "test_runner.hpp"
#include "../modules/892_numerical_kernel/numerical_kernel.hpp"

namespace {

testing::TestOutcome run_numerical_kernel() {
    numerical_kernel::init();
    auto s = numerical_kernel::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    numerical_kernel::shutdown();
    return testing::ok();
}

const int _reg_numerical_kernel = testing::register_test(
    "numerical_kernel",
    "892_numerical_kernel: stub status check",
    &run_numerical_kernel);

}
