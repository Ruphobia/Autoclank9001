// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/952_vendor_and_supplier_search/vendor_and_supplier_search.hpp"

namespace {

testing::TestOutcome run_vendor_and_supplier_search() {
    vendor_and_supplier_search::init();
    auto s = vendor_and_supplier_search::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    vendor_and_supplier_search::shutdown();
    return testing::ok();
}

const int _reg_vendor_and_supplier_search = testing::register_test(
    "vendor_and_supplier_search",
    "952_vendor_and_supplier_search: stub status check",
    &run_vendor_and_supplier_search);

}
