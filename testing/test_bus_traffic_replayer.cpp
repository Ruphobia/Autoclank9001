#include "test_runner.hpp"
#include "../modules/209_bus_traffic_replayer/bus_traffic_replayer.hpp"

namespace {

testing::TestOutcome run_bus_traffic_replayer() {
    bus_traffic_replayer::init();
    auto s = bus_traffic_replayer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    bus_traffic_replayer::shutdown();
    return testing::ok();
}

const int _reg_bus_traffic_replayer = testing::register_test(
    "bus_traffic_replayer",
    "209_bus_traffic_replayer: stub status check",
    &run_bus_traffic_replayer);

}
