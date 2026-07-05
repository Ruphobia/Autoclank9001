#include "test_runner.hpp"
#include "../modules/146_headless_browser/headless_browser.hpp"

namespace {

testing::TestOutcome run_headless_browser() {
    headless_browser::init();
    auto s = headless_browser::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    headless_browser::shutdown();
    return testing::ok();
}

const int _reg_headless_browser = testing::register_test(
    "headless_browser",
    "146_headless_browser: stub status check",
    &run_headless_browser);

}
