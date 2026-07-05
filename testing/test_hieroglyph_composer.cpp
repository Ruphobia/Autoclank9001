#include "test_runner.hpp"
#include "../modules/709_hieroglyph_composer/hieroglyph_composer.hpp"

namespace {

testing::TestOutcome run_hieroglyph_composer() {
    hieroglyph_composer::init();
    auto s = hieroglyph_composer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    hieroglyph_composer::shutdown();
    return testing::ok();
}

const int _reg_hieroglyph_composer = testing::register_test(
    "hieroglyph_composer",
    "709_hieroglyph_composer: stub status check",
    &run_hieroglyph_composer);

}
