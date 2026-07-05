#include "test_runner.hpp"
#include "../modules/1740_midi_router_and_monitor/midi_router_and_monitor.hpp"

namespace {

testing::TestOutcome run_midi_router_and_monitor() {
    midi_router_and_monitor::init();
    auto s = midi_router_and_monitor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    midi_router_and_monitor::shutdown();
    return testing::ok();
}

const int _reg_midi_router_and_monitor = testing::register_test(
    "midi_router_and_monitor",
    "1740_midi_router_and_monitor: stub status check",
    &run_midi_router_and_monitor);

}
