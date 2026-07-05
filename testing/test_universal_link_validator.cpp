#include "test_runner.hpp"
#include "../modules/169_universal_link_validator/universal_link_validator.hpp"

namespace {

testing::TestOutcome run_universal_link_validator() {
    universal_link_validator::init();
    auto s = universal_link_validator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    universal_link_validator::shutdown();
    return testing::ok();
}

const int _reg_universal_link_validator = testing::register_test(
    "universal_link_validator",
    "169_universal_link_validator: stub status check",
    &run_universal_link_validator);

}
