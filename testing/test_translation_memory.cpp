// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/828_translation_memory/translation_memory.hpp"

namespace {

testing::TestOutcome run_translation_memory() {
    translation_memory::init();
    auto s = translation_memory::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    translation_memory::shutdown();
    return testing::ok();
}

const int _reg_translation_memory = testing::register_test(
    "translation_memory",
    "828_translation_memory: stub status check",
    &run_translation_memory);

}
