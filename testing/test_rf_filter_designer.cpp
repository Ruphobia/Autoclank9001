#include "test_runner.hpp"
#include "../modules/1189_rf_filter_designer/rf_filter_designer.hpp"

namespace {

testing::TestOutcome run_rf_filter_designer() {
    rf_filter_designer::init();
    auto s = rf_filter_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    rf_filter_designer::shutdown();
    return testing::ok();
}

const int _reg_rf_filter_designer = testing::register_test(
    "rf_filter_designer",
    "1189_rf_filter_designer: stub status check",
    &run_rf_filter_designer);

}
