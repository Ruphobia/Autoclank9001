// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1244_recall_tsb_watcher/recall_tsb_watcher.hpp"

namespace {

testing::TestOutcome run_recall_tsb_watcher() {
    recall_tsb_watcher::init();
    auto s = recall_tsb_watcher::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    recall_tsb_watcher::shutdown();
    return testing::ok();
}

const int _reg_recall_tsb_watcher = testing::register_test(
    "recall_tsb_watcher",
    "1244_recall_tsb_watcher: stub status check",
    &run_recall_tsb_watcher);

}
