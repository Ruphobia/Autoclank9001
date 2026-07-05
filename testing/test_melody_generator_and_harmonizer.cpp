#include "test_runner.hpp"
#include "../modules/1716_melody_generator_and_harmonizer/melody_generator_and_harmonizer.hpp"

namespace {

testing::TestOutcome run_melody_generator_and_harmonizer() {
    melody_generator_and_harmonizer::init();
    auto s = melody_generator_and_harmonizer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    melody_generator_and_harmonizer::shutdown();
    return testing::ok();
}

const int _reg_melody_generator_and_harmonizer = testing::register_test(
    "melody_generator_and_harmonizer",
    "1716_melody_generator_and_harmonizer: stub status check",
    &run_melody_generator_and_harmonizer);

}
