#include "test_runner.hpp"
#include "../modules/1683_mtf_chart_analyzer/mtf_chart_analyzer.hpp"

namespace {

testing::TestOutcome run_mtf_chart_analyzer() {
    mtf_chart_analyzer::init();
    auto s = mtf_chart_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    mtf_chart_analyzer::shutdown();
    return testing::ok();
}

const int _reg_mtf_chart_analyzer = testing::register_test(
    "mtf_chart_analyzer",
    "1683_mtf_chart_analyzer: stub status check",
    &run_mtf_chart_analyzer);

}
