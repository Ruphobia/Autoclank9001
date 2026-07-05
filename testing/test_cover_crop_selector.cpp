#include "test_runner.hpp"
#include "../modules/1618_cover_crop_selector/cover_crop_selector.hpp"

namespace {

testing::TestOutcome run_cover_crop_selector() {
    cover_crop_selector::init();
    auto s = cover_crop_selector::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    cover_crop_selector::shutdown();
    return testing::ok();
}

const int _reg_cover_crop_selector = testing::register_test(
    "cover_crop_selector",
    "1618_cover_crop_selector: stub status check",
    &run_cover_crop_selector);

}
