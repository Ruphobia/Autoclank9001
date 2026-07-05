#include "test_runner.hpp"
#include "../modules/936_drawing_generator/drawing_generator.hpp"

namespace {

testing::TestOutcome run_drawing_generator() {
    drawing_generator::init();
    auto s = drawing_generator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    drawing_generator::shutdown();
    return testing::ok();
}

const int _reg_drawing_generator = testing::register_test(
    "drawing_generator",
    "936_drawing_generator: stub status check",
    &run_drawing_generator);

}
