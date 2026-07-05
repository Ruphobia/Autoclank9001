#include "test_runner.hpp"
#include "../modules/2191_body_scan_and_yoga_nidra_player/body_scan_and_yoga_nidra_player.hpp"

namespace {

testing::TestOutcome run_body_scan_and_yoga_nidra_player() {
    body_scan_and_yoga_nidra_player::init();
    auto s = body_scan_and_yoga_nidra_player::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    body_scan_and_yoga_nidra_player::shutdown();
    return testing::ok();
}

const int _reg_body_scan_and_yoga_nidra_player = testing::register_test(
    "body_scan_and_yoga_nidra_player",
    "2191_body_scan_and_yoga_nidra_player: stub status check",
    &run_body_scan_and_yoga_nidra_player);

}
