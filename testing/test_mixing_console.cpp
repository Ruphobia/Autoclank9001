#include "test_runner.hpp"
#include "../modules/1744_mixing_console/mixing_console.hpp"

namespace {

testing::TestOutcome run_mixing_console() {
    mixing_console::init();
    auto s = mixing_console::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    mixing_console::shutdown();
    return testing::ok();
}

const int _reg_mixing_console = testing::register_test(
    "mixing_console",
    "1744_mixing_console: stub status check",
    &run_mixing_console);

}
