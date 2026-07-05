// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/667_dataset_curator/dataset_curator.hpp"

namespace {

testing::TestOutcome run_dataset_curator() {
    dataset_curator::init();
    auto s = dataset_curator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    dataset_curator::shutdown();
    return testing::ok();
}

const int _reg_dataset_curator = testing::register_test(
    "dataset_curator",
    "667_dataset_curator: stub status check",
    &run_dataset_curator);

}
