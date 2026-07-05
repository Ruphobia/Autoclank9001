// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1640_transcoder/transcoder.hpp"

namespace {

testing::TestOutcome run_transcoder() {
    transcoder::init();
    auto s = transcoder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    transcoder::shutdown();
    return testing::ok();
}

const int _reg_transcoder = testing::register_test(
    "transcoder",
    "1640_transcoder: stub status check",
    &run_transcoder);

}
