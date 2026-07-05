// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/504_interview_transcriber/interview_transcriber.hpp"

namespace {

testing::TestOutcome run_interview_transcriber() {
    interview_transcriber::init();
    auto s = interview_transcriber::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    interview_transcriber::shutdown();
    return testing::ok();
}

const int _reg_interview_transcriber = testing::register_test(
    "interview_transcriber",
    "504_interview_transcriber: stub status check",
    &run_interview_transcriber);

}
