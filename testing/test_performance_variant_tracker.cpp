#include "test_runner.hpp"
#include "../modules/783_performance_variant_tracker/performance_variant_tracker.hpp"

namespace {

testing::TestOutcome run_performance_variant_tracker() {
    performance_variant_tracker::init();
    auto s = performance_variant_tracker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    performance_variant_tracker::shutdown();
    return testing::ok();
}

const int _reg_performance_variant_tracker = testing::register_test(
    "performance_variant_tracker",
    "783_performance_variant_tracker: stub status check",
    &run_performance_variant_tracker);

}
