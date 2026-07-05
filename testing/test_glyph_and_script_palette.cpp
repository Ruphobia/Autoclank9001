#include "test_runner.hpp"
#include "../modules/710_glyph_and_script_palette/glyph_and_script_palette.hpp"

namespace {

testing::TestOutcome run_glyph_and_script_palette() {
    glyph_and_script_palette::init();
    auto s = glyph_and_script_palette::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    glyph_and_script_palette::shutdown();
    return testing::ok();
}

const int _reg_glyph_and_script_palette = testing::register_test(
    "glyph_and_script_palette",
    "710_glyph_and_script_palette: stub status check",
    &run_glyph_and_script_palette);

}
