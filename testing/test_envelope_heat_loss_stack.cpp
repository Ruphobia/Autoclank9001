#include "test_runner.hpp"
#include "../modules/2028_envelope_heat_loss_stack/envelope_heat_loss_stack.hpp"

namespace {

testing::TestOutcome run_envelope_heat_loss_stack() {
    envelope_heat_loss_stack::init();
    auto s = envelope_heat_loss_stack::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    envelope_heat_loss_stack::shutdown();
    return testing::ok();
}

const int _reg_envelope_heat_loss_stack = testing::register_test(
    "envelope_heat_loss_stack",
    "2028_envelope_heat_loss_stack: stub status check",
    &run_envelope_heat_loss_stack);

}
