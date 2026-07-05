// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1510_track_and_sign_identifier/track_and_sign_identifier.hpp"

namespace {

testing::TestOutcome run_track_and_sign_identifier() {
    track_and_sign_identifier::init();
    auto s = track_and_sign_identifier::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    track_and_sign_identifier::shutdown();
    return testing::ok();
}

const int _reg_track_and_sign_identifier = testing::register_test(
    "track_and_sign_identifier",
    "1510_track_and_sign_identifier: stub status check",
    &run_track_and_sign_identifier);

}
