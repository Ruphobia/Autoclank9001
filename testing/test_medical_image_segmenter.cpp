#include "test_runner.hpp"
#include "../modules/1413_medical_image_segmenter/medical_image_segmenter.hpp"

namespace {

testing::TestOutcome run_medical_image_segmenter() {
    medical_image_segmenter::init();
    auto s = medical_image_segmenter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    medical_image_segmenter::shutdown();
    return testing::ok();
}

const int _reg_medical_image_segmenter = testing::register_test(
    "medical_image_segmenter",
    "1413_medical_image_segmenter: stub status check",
    &run_medical_image_segmenter);

}
