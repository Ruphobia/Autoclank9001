#include "test_runner.hpp"
#include "../modules/968_print_failure_diagnostician/print_failure_diagnostician.hpp"

namespace {

testing::TestOutcome run_print_failure_diagnostician() {
    print_failure_diagnostician::init();
    auto s = print_failure_diagnostician::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    print_failure_diagnostician::shutdown();
    return testing::ok();
}

const int _reg_print_failure_diagnostician = testing::register_test(
    "print_failure_diagnostician",
    "968_print_failure_diagnostician: stub status check",
    &run_print_failure_diagnostician);

}
