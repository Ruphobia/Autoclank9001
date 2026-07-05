#include "test_runner.hpp"
#include "../modules/1899_needle_and_cartridge_reference/needle_and_cartridge_reference.hpp"

namespace {

testing::TestOutcome run_needle_and_cartridge_reference() {
    needle_and_cartridge_reference::init();
    auto s = needle_and_cartridge_reference::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    needle_and_cartridge_reference::shutdown();
    return testing::ok();
}

const int _reg_needle_and_cartridge_reference = testing::register_test(
    "needle_and_cartridge_reference",
    "1899_needle_and_cartridge_reference: stub status check",
    &run_needle_and_cartridge_reference);

}
