#include "test_runner.hpp"
#include "../modules/1460_mrms_mosaic_browser/mrms_mosaic_browser.hpp"

namespace {

testing::TestOutcome run_mrms_mosaic_browser() {
    mrms_mosaic_browser::init();
    auto s = mrms_mosaic_browser::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    mrms_mosaic_browser::shutdown();
    return testing::ok();
}

const int _reg_mrms_mosaic_browser = testing::register_test(
    "mrms_mosaic_browser",
    "1460_mrms_mosaic_browser: stub status check",
    &run_mrms_mosaic_browser);

}
