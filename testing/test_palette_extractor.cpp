#include "test_runner.hpp"
#include "../modules/1861_palette_extractor/palette_extractor.hpp"

namespace {

testing::TestOutcome run_palette_extractor() {
    palette_extractor::init();
    auto s = palette_extractor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    palette_extractor::shutdown();
    return testing::ok();
}

const int _reg_palette_extractor = testing::register_test(
    "palette_extractor",
    "1861_palette_extractor: stub status check",
    &run_palette_extractor);

}
