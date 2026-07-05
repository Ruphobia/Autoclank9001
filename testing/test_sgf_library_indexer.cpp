// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1969_sgf_library_indexer/sgf_library_indexer.hpp"

namespace {

testing::TestOutcome run_sgf_library_indexer() {
    sgf_library_indexer::init();
    auto s = sgf_library_indexer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    sgf_library_indexer::shutdown();
    return testing::ok();
}

const int _reg_sgf_library_indexer = testing::register_test(
    "sgf_library_indexer",
    "1969_sgf_library_indexer: stub status check",
    &run_sgf_library_indexer);

}
