#include "test_runner.hpp"
#include "../modules/1263_computer_vision_pipeline/computer_vision_pipeline.hpp"

namespace {

testing::TestOutcome run_computer_vision_pipeline() {
    computer_vision_pipeline::init();
    auto s = computer_vision_pipeline::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    computer_vision_pipeline::shutdown();
    return testing::ok();
}

const int _reg_computer_vision_pipeline = testing::register_test(
    "computer_vision_pipeline",
    "1263_computer_vision_pipeline: stub status check",
    &run_computer_vision_pipeline);

}
