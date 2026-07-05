#include "test_runner.hpp"
#include "../modules/383_address_toolkit/address_toolkit.hpp"

namespace {

testing::TestOutcome run_address_toolkit() {
    address_toolkit::init();
    auto s = address_toolkit::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    address_toolkit::shutdown();
    return testing::ok();
}

const int _reg_address_toolkit = testing::register_test(
    "address_toolkit",
    "383_address_toolkit: stub status check",
    &run_address_toolkit);

}
