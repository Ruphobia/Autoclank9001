#include "test_runner.hpp"
#include "../modules/1232_torque_spec_fastener_lookup/torque_spec_fastener_lookup.hpp"

namespace {

testing::TestOutcome run_torque_spec_fastener_lookup() {
    torque_spec_fastener_lookup::init();
    auto s = torque_spec_fastener_lookup::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    torque_spec_fastener_lookup::shutdown();
    return testing::ok();
}

const int _reg_torque_spec_fastener_lookup = testing::register_test(
    "torque_spec_fastener_lookup",
    "1232_torque_spec_fastener_lookup: stub status check",
    &run_torque_spec_fastener_lookup);

}
