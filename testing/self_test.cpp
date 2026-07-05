// Sanity test for the test harness itself. Registers via a static
// initializer to confirm the registry, CLI dispatch, and OK reporting all
// work end to end before any module-specific test is added.

#include "test_runner.hpp"

namespace {

testing::TestOutcome harness_self_test() {
    if (testing::registry().empty()) {
        return testing::fail("registry empty during self-test");
    }
    return testing::ok();
}

const int _harness_self_reg = testing::register_test(
    "harness",
    "test runner registry, CLI dispatch, OK reporting",
    &harness_self_test);

}
