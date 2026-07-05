// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1488_catalog_browser/catalog_browser.hpp"

namespace {

testing::TestOutcome run_catalog_browser() {
    catalog_browser::init();
    auto s = catalog_browser::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    catalog_browser::shutdown();
    return testing::ok();
}

const int _reg_catalog_browser = testing::register_test(
    "catalog_browser",
    "1488_catalog_browser: stub status check",
    &run_catalog_browser);

}
