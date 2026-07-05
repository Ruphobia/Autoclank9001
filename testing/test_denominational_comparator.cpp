#include "test_runner.hpp"
#include "../modules/742_denominational_comparator/denominational_comparator.hpp"

namespace {

testing::TestOutcome run_denominational_comparator() {
    denominational_comparator::init();
    auto s = denominational_comparator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    denominational_comparator::shutdown();
    return testing::ok();
}

const int _reg_denominational_comparator = testing::register_test(
    "denominational_comparator",
    "742_denominational_comparator: stub status check",
    &run_denominational_comparator);

}
