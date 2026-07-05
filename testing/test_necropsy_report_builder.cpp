#include "test_runner.hpp"
#include "../modules/1504_necropsy_report_builder/necropsy_report_builder.hpp"

namespace {

testing::TestOutcome run_necropsy_report_builder() {
    necropsy_report_builder::init();
    auto s = necropsy_report_builder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    necropsy_report_builder::shutdown();
    return testing::ok();
}

const int _reg_necropsy_report_builder = testing::register_test(
    "necropsy_report_builder",
    "1504_necropsy_report_builder: stub status check",
    &run_necropsy_report_builder);

}
