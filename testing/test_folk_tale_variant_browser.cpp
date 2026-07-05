#include "test_runner.hpp"
#include "../modules/769_folk_tale_variant_browser/folk_tale_variant_browser.hpp"

namespace {

testing::TestOutcome run_folk_tale_variant_browser() {
    folk_tale_variant_browser::init();
    auto s = folk_tale_variant_browser::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    folk_tale_variant_browser::shutdown();
    return testing::ok();
}

const int _reg_folk_tale_variant_browser = testing::register_test(
    "folk_tale_variant_browser",
    "769_folk_tale_variant_browser: stub status check",
    &run_folk_tale_variant_browser);

}
