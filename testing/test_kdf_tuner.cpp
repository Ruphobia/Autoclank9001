// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/376_kdf_tuner/kdf_tuner.hpp"

namespace {

testing::TestOutcome run_kdf_tuner() {
    kdf_tuner::init();
    auto s = kdf_tuner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    kdf_tuner::shutdown();
    return testing::ok();
}

const int _reg_kdf_tuner = testing::register_test(
    "kdf_tuner",
    "376_kdf_tuner: stub status check",
    &run_kdf_tuner);

}
