#include "test_runner.hpp"
#include "../modules/1192_sdr_receiver_console/sdr_receiver_console.hpp"

namespace {

testing::TestOutcome run_sdr_receiver_console() {
    sdr_receiver_console::init();
    auto s = sdr_receiver_console::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    sdr_receiver_console::shutdown();
    return testing::ok();
}

const int _reg_sdr_receiver_console = testing::register_test(
    "sdr_receiver_console",
    "1192_sdr_receiver_console: stub status check",
    &run_sdr_receiver_console);

}
