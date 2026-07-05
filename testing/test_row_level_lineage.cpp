#include "test_runner.hpp"
#include "../modules/245_row_level_lineage/row_level_lineage.hpp"

namespace {

testing::TestOutcome run_row_level_lineage() {
    row_level_lineage::init();
    auto s = row_level_lineage::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    row_level_lineage::shutdown();
    return testing::ok();
}

const int _reg_row_level_lineage = testing::register_test(
    "row_level_lineage",
    "245_row_level_lineage: stub status check",
    &run_row_level_lineage);

}
