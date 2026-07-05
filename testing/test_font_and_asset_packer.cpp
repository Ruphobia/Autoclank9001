#include "test_runner.hpp"
#include "../modules/212_font_and_asset_packer/font_and_asset_packer.hpp"

namespace {

testing::TestOutcome run_font_and_asset_packer() {
    font_and_asset_packer::init();
    auto s = font_and_asset_packer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    font_and_asset_packer::shutdown();
    return testing::ok();
}

const int _reg_font_and_asset_packer = testing::register_test(
    "font_and_asset_packer",
    "212_font_and_asset_packer: stub status check",
    &run_font_and_asset_packer);

}
