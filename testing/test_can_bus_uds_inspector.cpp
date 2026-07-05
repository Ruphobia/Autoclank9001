#include "test_runner.hpp"
#include "../modules/1234_can_bus_uds_inspector/can_bus_uds_inspector.hpp"

namespace {

testing::TestOutcome run_can_bus_uds_inspector() {
    can_bus_uds_inspector::init();
    auto s = can_bus_uds_inspector::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    can_bus_uds_inspector::shutdown();
    return testing::ok();
}

const int _reg_can_bus_uds_inspector = testing::register_test(
    "can_bus_uds_inspector",
    "1234_can_bus_uds_inspector: stub status check",
    &run_can_bus_uds_inspector);

}
