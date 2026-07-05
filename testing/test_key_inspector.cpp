#include "test_runner.hpp"
#include "../modules/372_key_inspector/key_inspector.hpp"

namespace {

testing::TestOutcome run_key_inspector() {
    key_inspector::init();
    auto s = key_inspector::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    key_inspector::shutdown();
    return testing::ok();
}

const int _reg_key_inspector = testing::register_test(
    "key_inspector",
    "372_key_inspector: stub status check",
    &run_key_inspector);

}
