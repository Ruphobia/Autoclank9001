#include "test_runner.hpp"
#include "../modules/1478_image_stacker/image_stacker.hpp"

namespace {

testing::TestOutcome run_image_stacker() {
    image_stacker::init();
    auto s = image_stacker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    image_stacker::shutdown();
    return testing::ok();
}

const int _reg_image_stacker = testing::register_test(
    "image_stacker",
    "1478_image_stacker: stub status check",
    &run_image_stacker);

}
