#include "test_runner.hpp"
#include "../modules/721_parallel_versions_browser/parallel_versions_browser.hpp"

namespace {

testing::TestOutcome run_parallel_versions_browser() {
    parallel_versions_browser::init();
    auto s = parallel_versions_browser::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    parallel_versions_browser::shutdown();
    return testing::ok();
}

const int _reg_parallel_versions_browser = testing::register_test(
    "parallel_versions_browser",
    "721_parallel_versions_browser: stub status check",
    &run_parallel_versions_browser);

}
