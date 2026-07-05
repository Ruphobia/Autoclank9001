#include "test_runner.hpp"
#include "../modules/1682_print_soft_proofer/print_soft_proofer.hpp"

namespace {

testing::TestOutcome run_print_soft_proofer() {
    print_soft_proofer::init();
    auto s = print_soft_proofer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    print_soft_proofer::shutdown();
    return testing::ok();
}

const int _reg_print_soft_proofer = testing::register_test(
    "print_soft_proofer",
    "1682_print_soft_proofer: stub status check",
    &run_print_soft_proofer);

}
