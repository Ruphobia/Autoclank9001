#include "test_runner.hpp"
#include "../modules/410_vintage_serial_terminal/vintage_serial_terminal.hpp"

namespace {

testing::TestOutcome run_vintage_serial_terminal() {
    vintage_serial_terminal::init();
    auto s = vintage_serial_terminal::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    vintage_serial_terminal::shutdown();
    return testing::ok();
}

const int _reg_vintage_serial_terminal = testing::register_test(
    "vintage_serial_terminal",
    "410_vintage_serial_terminal: stub status check",
    &run_vintage_serial_terminal);

}
