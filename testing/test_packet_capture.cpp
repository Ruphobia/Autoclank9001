#include "test_runner.hpp"
#include "../modules/139_packet_capture/packet_capture.hpp"

namespace {

testing::TestOutcome run_packet_capture() {
    packet_capture::init();
    auto s = packet_capture::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    packet_capture::shutdown();
    return testing::ok();
}

const int _reg_packet_capture = testing::register_test(
    "packet_capture",
    "139_packet_capture: stub status check",
    &run_packet_capture);

}
