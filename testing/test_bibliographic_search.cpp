// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1538_bibliographic_search/bibliographic_search.hpp"

namespace {

testing::TestOutcome run_bibliographic_search() {
    bibliographic_search::init();
    auto s = bibliographic_search::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    bibliographic_search::shutdown();
    return testing::ok();
}

const int _reg_bibliographic_search = testing::register_test(
    "bibliographic_search",
    "1538_bibliographic_search: stub status check",
    &run_bibliographic_search);

}
