// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/711_ipa_reconstruction_lab/ipa_reconstruction_lab.hpp"

namespace {

testing::TestOutcome run_ipa_reconstruction_lab() {
    ipa_reconstruction_lab::init();
    auto s = ipa_reconstruction_lab::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    ipa_reconstruction_lab::shutdown();
    return testing::ok();
}

const int _reg_ipa_reconstruction_lab = testing::register_test(
    "ipa_reconstruction_lab",
    "711_ipa_reconstruction_lab: stub status check",
    &run_ipa_reconstruction_lab);

}
