#include "test_runner.hpp"
#include "../modules/467_hazop_and_lopa_workbook/hazop_and_lopa_workbook.hpp"

namespace {

testing::TestOutcome run_hazop_and_lopa_workbook() {
    hazop_and_lopa_workbook::init();
    auto s = hazop_and_lopa_workbook::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    hazop_and_lopa_workbook::shutdown();
    return testing::ok();
}

const int _reg_hazop_and_lopa_workbook = testing::register_test(
    "hazop_and_lopa_workbook",
    "467_hazop_and_lopa_workbook: stub status check",
    &run_hazop_and_lopa_workbook);

}
