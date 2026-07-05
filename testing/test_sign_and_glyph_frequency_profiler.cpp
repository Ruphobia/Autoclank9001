#include "test_runner.hpp"
#include "../modules/719_sign_and_glyph_frequency_profiler/sign_and_glyph_frequency_profiler.hpp"

namespace {

testing::TestOutcome run_sign_and_glyph_frequency_profiler() {
    sign_and_glyph_frequency_profiler::init();
    auto s = sign_and_glyph_frequency_profiler::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    sign_and_glyph_frequency_profiler::shutdown();
    return testing::ok();
}

const int _reg_sign_and_glyph_frequency_profiler = testing::register_test(
    "sign_and_glyph_frequency_profiler",
    "719_sign_and_glyph_frequency_profiler: stub status check",
    &run_sign_and_glyph_frequency_profiler);

}
