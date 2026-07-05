#include "test_runner.hpp"
#include "../modules/1778_stitch_library/stitch_library.hpp"

namespace {

testing::TestOutcome run_stitch_library() {
    stitch_library::init();
    auto s = stitch_library::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    stitch_library::shutdown();
    return testing::ok();
}

const int _reg_stitch_library = testing::register_test(
    "stitch_library",
    "1778_stitch_library: stub status check",
    &run_stitch_library);

}
