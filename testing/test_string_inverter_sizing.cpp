#include "test_runner.hpp"
#include "../modules/1276_string_inverter_sizing/string_inverter_sizing.hpp"

namespace {

testing::TestOutcome run_string_inverter_sizing() {
    string_inverter_sizing::init();
    auto s = string_inverter_sizing::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    string_inverter_sizing::shutdown();
    return testing::ok();
}

const int _reg_string_inverter_sizing = testing::register_test(
    "string_inverter_sizing",
    "1276_string_inverter_sizing: stub status check",
    &run_string_inverter_sizing);

}
