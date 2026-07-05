#include "test_runner.hpp"
#include "../modules/085_ocr_and_scan_cleanup/ocr_and_scan_cleanup.hpp"

namespace {

testing::TestOutcome run_ocr_and_scan_cleanup() {
    ocr_and_scan_cleanup::init();
    auto s = ocr_and_scan_cleanup::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    ocr_and_scan_cleanup::shutdown();
    return testing::ok();
}

const int _reg_ocr_and_scan_cleanup = testing::register_test(
    "ocr_and_scan_cleanup",
    "085_ocr_and_scan_cleanup: stub status check",
    &run_ocr_and_scan_cleanup);

}
