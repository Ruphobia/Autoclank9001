#include "test_runner.hpp"
#include "../modules/308_bitmap_font_forge/bitmap_font_forge.hpp"

namespace {

testing::TestOutcome run_bitmap_font_forge() {
    bitmap_font_forge::init();
    auto s = bitmap_font_forge::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    bitmap_font_forge::shutdown();
    return testing::ok();
}

const int _reg_bitmap_font_forge = testing::register_test(
    "bitmap_font_forge",
    "308_bitmap_font_forge: stub status check",
    &run_bitmap_font_forge);

}
