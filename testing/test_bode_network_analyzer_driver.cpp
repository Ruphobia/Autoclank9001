#include "test_runner.hpp"
#include "../modules/1747_bode_network_analyzer_driver/bode_network_analyzer_driver.hpp"

namespace {

testing::TestOutcome run_bode_network_analyzer_driver() {
    bode_network_analyzer_driver::init();
    auto s = bode_network_analyzer_driver::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    bode_network_analyzer_driver::shutdown();
    return testing::ok();
}

const int _reg_bode_network_analyzer_driver = testing::register_test(
    "bode_network_analyzer_driver",
    "1747_bode_network_analyzer_driver: stub status check",
    &run_bode_network_analyzer_driver);

}
