#include "test_runner.hpp"
#include "../modules/1199_obd_ii_live_console/obd_ii_live_console.hpp"

namespace {

testing::TestOutcome run_obd_ii_live_console() {
    obd_ii_live_console::init();
    auto s = obd_ii_live_console::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    obd_ii_live_console::shutdown();
    return testing::ok();
}

const int _reg_obd_ii_live_console = testing::register_test(
    "obd_ii_live_console",
    "1199_obd_ii_live_console: stub status check",
    &run_obd_ii_live_console);

}
