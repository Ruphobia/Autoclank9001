// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/666_feature_store/feature_store.hpp"

namespace {

testing::TestOutcome run_feature_store() {
    feature_store::init();
    auto s = feature_store::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    feature_store::shutdown();
    return testing::ok();
}

const int _reg_feature_store = testing::register_test(
    "feature_store",
    "666_feature_store: stub status check",
    &run_feature_store);

}
