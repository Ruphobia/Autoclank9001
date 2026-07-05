#include "test_runner.hpp"
#include "../modules/1069_leak_locator/leak_locator.hpp"

namespace {

testing::TestOutcome run_leak_locator() {
    leak_locator::init();
    auto s = leak_locator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    leak_locator::shutdown();
    return testing::ok();
}

const int _reg_leak_locator = testing::register_test(
    "leak_locator",
    "1069_leak_locator: stub status check",
    &run_leak_locator);

}
