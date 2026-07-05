#include "test_runner.hpp"
#include "../modules/1624_image_generator/image_generator.hpp"

namespace {

testing::TestOutcome run_image_generator() {
    image_generator::init();
    auto s = image_generator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    image_generator::shutdown();
    return testing::ok();
}

const int _reg_image_generator = testing::register_test(
    "image_generator",
    "1624_image_generator: stub status check",
    &run_image_generator);

}
