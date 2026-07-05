#include "test_runner.hpp"
#include "../modules/1707_lead_sheet_generator/lead_sheet_generator.hpp"

namespace {

testing::TestOutcome run_lead_sheet_generator() {
    lead_sheet_generator::init();
    auto s = lead_sheet_generator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    lead_sheet_generator::shutdown();
    return testing::ok();
}

const int _reg_lead_sheet_generator = testing::register_test(
    "lead_sheet_generator",
    "1707_lead_sheet_generator: stub status check",
    &run_lead_sheet_generator);

}
