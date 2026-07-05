// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/604_face_matcher/face_matcher.hpp"

namespace {

testing::TestOutcome run_face_matcher() {
    face_matcher::init();
    auto s = face_matcher::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    face_matcher::shutdown();
    return testing::ok();
}

const int _reg_face_matcher = testing::register_test(
    "face_matcher",
    "604_face_matcher: stub status check",
    &run_face_matcher);

}
