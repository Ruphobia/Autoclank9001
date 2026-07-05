#include "test_runner.hpp"
#include "../modules/823_ocr_studio/ocr_studio.hpp"

namespace {

testing::TestOutcome run_ocr_studio() {
    ocr_studio::init();
    auto s = ocr_studio::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    ocr_studio::shutdown();
    return testing::ok();
}

const int _reg_ocr_studio = testing::register_test(
    "ocr_studio",
    "823_ocr_studio: stub status check",
    &run_ocr_studio);

}
