#include "test_runner.hpp"
#include "../modules/894_constants_reference/constants_reference.hpp"

namespace {

testing::TestOutcome run_constants_reference() {
    constants_reference::init();
    auto s = constants_reference::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    constants_reference::shutdown();
    return testing::ok();
}

const int _reg_constants_reference = testing::register_test(
    "constants_reference",
    "894_constants_reference: stub status check",
    &run_constants_reference);

}
