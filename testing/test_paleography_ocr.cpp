#include "test_runner.hpp"
#include "../modules/2136_paleography_ocr/paleography_ocr.hpp"

namespace {

testing::TestOutcome run_paleography_ocr() {
    paleography_ocr::init();
    auto s = paleography_ocr::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    paleography_ocr::shutdown();
    return testing::ok();
}

const int _reg_paleography_ocr = testing::register_test(
    "paleography_ocr",
    "2136_paleography_ocr: stub status check",
    &run_paleography_ocr);

}
