#include "test_runner.hpp"
#include "../modules/405_filesystem_browser/filesystem_browser.hpp"

namespace {

testing::TestOutcome run_filesystem_browser() {
    filesystem_browser::init();
    auto s = filesystem_browser::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    filesystem_browser::shutdown();
    return testing::ok();
}

const int _reg_filesystem_browser = testing::register_test(
    "filesystem_browser",
    "405_filesystem_browser: stub status check",
    &run_filesystem_browser);

}
