#include "test_runner.hpp"
#include "../modules/270_packet_inspector/packet_inspector.hpp"

namespace {

testing::TestOutcome run_packet_inspector() {
    packet_inspector::init();
    auto s = packet_inspector::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    packet_inspector::shutdown();
    return testing::ok();
}

const int _reg_packet_inspector = testing::register_test(
    "packet_inspector",
    "270_packet_inspector: stub status check",
    &run_packet_inspector);

}
