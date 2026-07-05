#include "test_runner.hpp"
#include "../modules/1879_catalog_reference_browser/catalog_reference_browser.hpp"

namespace {

testing::TestOutcome run_catalog_reference_browser() {
    catalog_reference_browser::init();
    auto s = catalog_reference_browser::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    catalog_reference_browser::shutdown();
    return testing::ok();
}

const int _reg_catalog_reference_browser = testing::register_test(
    "catalog_reference_browser",
    "1879_catalog_reference_browser: stub status check",
    &run_catalog_reference_browser);

}
