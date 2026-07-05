// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/803_lemmatizer_pos_lab/lemmatizer_pos_lab.hpp"

namespace {

testing::TestOutcome run_lemmatizer_pos_lab() {
    lemmatizer_pos_lab::init();
    auto s = lemmatizer_pos_lab::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    lemmatizer_pos_lab::shutdown();
    return testing::ok();
}

const int _reg_lemmatizer_pos_lab = testing::register_test(
    "lemmatizer_pos_lab",
    "803_lemmatizer_pos_lab: stub status check",
    &run_lemmatizer_pos_lab);

}
