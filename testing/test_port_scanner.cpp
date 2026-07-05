#include "test_runner.hpp"
#include "../modules/143_port_scanner/port_scanner.hpp"

namespace {

testing::TestOutcome run_port_scanner() {
    port_scanner::init();
    auto s = port_scanner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    port_scanner::shutdown();
    return testing::ok();
}

const int _reg_port_scanner = testing::register_test(
    "port_scanner",
    "143_port_scanner: stub status check",
    &run_port_scanner);

}
