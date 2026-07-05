#include "test_runner.hpp"
#include "../modules/1213_corner_weight_and_setup_sheet/corner_weight_and_setup_sheet.hpp"

namespace {

testing::TestOutcome run_corner_weight_and_setup_sheet() {
    corner_weight_and_setup_sheet::init();
    auto s = corner_weight_and_setup_sheet::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    corner_weight_and_setup_sheet::shutdown();
    return testing::ok();
}

const int _reg_corner_weight_and_setup_sheet = testing::register_test(
    "corner_weight_and_setup_sheet",
    "1213_corner_weight_and_setup_sheet: stub status check",
    &run_corner_weight_and_setup_sheet);

}
