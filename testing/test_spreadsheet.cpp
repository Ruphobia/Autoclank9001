#include "test_runner.hpp"
#include "../modules/413_spreadsheet/spreadsheet.hpp"

namespace {

testing::TestOutcome run_spreadsheet() {
    spreadsheet::init();
    auto s = spreadsheet::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    spreadsheet::shutdown();
    return testing::ok();
}

const int _reg_spreadsheet = testing::register_test(
    "spreadsheet",
    "413_spreadsheet: stub status check",
    &run_spreadsheet);

}
