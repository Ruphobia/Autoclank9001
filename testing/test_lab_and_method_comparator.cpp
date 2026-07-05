#include "test_runner.hpp"
#include "../modules/644_lab_and_method_comparator/lab_and_method_comparator.hpp"

namespace {

testing::TestOutcome run_lab_and_method_comparator() {
    lab_and_method_comparator::init();
    auto s = lab_and_method_comparator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    lab_and_method_comparator::shutdown();
    return testing::ok();
}

const int _reg_lab_and_method_comparator = testing::register_test(
    "lab_and_method_comparator",
    "644_lab_and_method_comparator: stub status check",
    &run_lab_and_method_comparator);

}
