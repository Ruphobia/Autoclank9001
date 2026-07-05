#include "test_runner.hpp"
#include "../modules/142_traceroute_and_latency_map/traceroute_and_latency_map.hpp"

namespace {

testing::TestOutcome run_traceroute_and_latency_map() {
    traceroute_and_latency_map::init();
    auto s = traceroute_and_latency_map::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    traceroute_and_latency_map::shutdown();
    return testing::ok();
}

const int _reg_traceroute_and_latency_map = testing::register_test(
    "traceroute_and_latency_map",
    "142_traceroute_and_latency_map: stub status check",
    &run_traceroute_and_latency_map);

}
