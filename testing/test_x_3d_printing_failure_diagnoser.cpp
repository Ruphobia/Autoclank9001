#include "test_runner.hpp"
#include "../modules/944_x_3d_printing_failure_diagnoser/x_3d_printing_failure_diagnoser.hpp"

namespace {

testing::TestOutcome run_x_3d_printing_failure_diagnoser() {
    x_3d_printing_failure_diagnoser::init();
    auto s = x_3d_printing_failure_diagnoser::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    x_3d_printing_failure_diagnoser::shutdown();
    return testing::ok();
}

const int _reg_x_3d_printing_failure_diagnoser = testing::register_test(
    "x_3d_printing_failure_diagnoser",
    "944_x_3d_printing_failure_diagnoser: stub status check",
    &run_x_3d_printing_failure_diagnoser);

}
