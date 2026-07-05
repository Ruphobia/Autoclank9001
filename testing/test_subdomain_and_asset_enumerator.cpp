// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/154_subdomain_and_asset_enumerator/subdomain_and_asset_enumerator.hpp"

namespace {

testing::TestOutcome run_subdomain_and_asset_enumerator() {
    subdomain_and_asset_enumerator::init();
    auto s = subdomain_and_asset_enumerator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    subdomain_and_asset_enumerator::shutdown();
    return testing::ok();
}

const int _reg_subdomain_and_asset_enumerator = testing::register_test(
    "subdomain_and_asset_enumerator",
    "154_subdomain_and_asset_enumerator: stub status check",
    &run_subdomain_and_asset_enumerator);

}
