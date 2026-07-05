#include "test_runner.hpp"
#include "../modules/1999_input_lab/input_lab.hpp"

namespace {

testing::TestOutcome run_input_lab() {
    input_lab::init();
    auto s = input_lab::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    input_lab::shutdown();
    return testing::ok();
}

const int _reg_input_lab = testing::register_test(
    "input_lab",
    "1999_input_lab: stub status check",
    &run_input_lab);

}
