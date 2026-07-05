#include "test_runner.hpp"
#include "../modules/2148_photo_restoration_and_face_matcher/photo_restoration_and_face_matcher.hpp"

namespace {

testing::TestOutcome run_photo_restoration_and_face_matcher() {
    photo_restoration_and_face_matcher::init();
    auto s = photo_restoration_and_face_matcher::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    photo_restoration_and_face_matcher::shutdown();
    return testing::ok();
}

const int _reg_photo_restoration_and_face_matcher = testing::register_test(
    "photo_restoration_and_face_matcher",
    "2148_photo_restoration_and_face_matcher: stub status check",
    &run_photo_restoration_and_face_matcher);

}
