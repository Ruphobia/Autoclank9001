// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/241_search_index_manager/search_index_manager.hpp"

namespace {

testing::TestOutcome run_search_index_manager() {
    search_index_manager::init();
    auto s = search_index_manager::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    search_index_manager::shutdown();
    return testing::ok();
}

const int _reg_search_index_manager = testing::register_test(
    "search_index_manager",
    "241_search_index_manager: stub status check",
    &run_search_index_manager);

}
