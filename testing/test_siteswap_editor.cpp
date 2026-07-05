#include "test_runner.hpp"
#include "../modules/1932_siteswap_editor/siteswap_editor.hpp"

namespace {

testing::TestOutcome run_siteswap_editor() {
    siteswap_editor::init();
    auto s = siteswap_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    siteswap_editor::shutdown();
    return testing::ok();
}

const int _reg_siteswap_editor = testing::register_test(
    "siteswap_editor",
    "1932_siteswap_editor: stub status check",
    &run_siteswap_editor);

}
