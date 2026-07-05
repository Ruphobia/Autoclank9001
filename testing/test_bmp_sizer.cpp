#include "test_runner.hpp"
#include "../modules/1063_bmp_sizer/bmp_sizer.hpp"

namespace {

testing::TestOutcome run_bmp_sizer() {
    bmp_sizer::init();
    auto s = bmp_sizer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    bmp_sizer::shutdown();
    return testing::ok();
}

const int _reg_bmp_sizer = testing::register_test(
    "bmp_sizer",
    "1063_bmp_sizer: stub status check",
    &run_bmp_sizer);

}
