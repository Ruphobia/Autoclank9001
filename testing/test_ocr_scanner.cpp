#include "test_runner.hpp"
#include "../modules/433_ocr_scanner/ocr_scanner.hpp"

namespace {

testing::TestOutcome run_ocr_scanner() {
    ocr_scanner::init();
    auto s = ocr_scanner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    ocr_scanner::shutdown();
    return testing::ok();
}

const int _reg_ocr_scanner = testing::register_test(
    "ocr_scanner",
    "433_ocr_scanner: stub status check",
    &run_ocr_scanner);

}
