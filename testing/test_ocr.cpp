#include "test_runner.hpp"
#include "../modules/1632_ocr/ocr.hpp"

namespace {

testing::TestOutcome run_ocr() {
    ocr::init();
    auto s = ocr::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    ocr::shutdown();
    return testing::ok();
}

const int _reg_ocr = testing::register_test(
    "ocr",
    "1632_ocr: stub status check",
    &run_ocr);

}
