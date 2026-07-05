#include "test_runner.hpp"
#include "../modules/1346_microscopy_image_analyzer/microscopy_image_analyzer.hpp"

namespace {

testing::TestOutcome run_microscopy_image_analyzer() {
    microscopy_image_analyzer::init();
    auto s = microscopy_image_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    microscopy_image_analyzer::shutdown();
    return testing::ok();
}

const int _reg_microscopy_image_analyzer = testing::register_test(
    "microscopy_image_analyzer",
    "1346_microscopy_image_analyzer: stub status check",
    &run_microscopy_image_analyzer);

}
