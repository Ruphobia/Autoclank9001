#include "test_runner.hpp"
#include "../modules/1891_watermark_and_perforation_analyzer/watermark_and_perforation_analyzer.hpp"

namespace {

testing::TestOutcome run_watermark_and_perforation_analyzer() {
    watermark_and_perforation_analyzer::init();
    auto s = watermark_and_perforation_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    watermark_and_perforation_analyzer::shutdown();
    return testing::ok();
}

const int _reg_watermark_and_perforation_analyzer = testing::register_test(
    "watermark_and_perforation_analyzer",
    "1891_watermark_and_perforation_analyzer: stub status check",
    &run_watermark_and_perforation_analyzer);

}
