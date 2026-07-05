#include "test_runner.hpp"
#include "../modules/1397_stimulus_presentation_builder/stimulus_presentation_builder.hpp"

namespace {

testing::TestOutcome run_stimulus_presentation_builder() {
    stimulus_presentation_builder::init();
    auto s = stimulus_presentation_builder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    stimulus_presentation_builder::shutdown();
    return testing::ok();
}

const int _reg_stimulus_presentation_builder = testing::register_test(
    "stimulus_presentation_builder",
    "1397_stimulus_presentation_builder: stub status check",
    &run_stimulus_presentation_builder);

}
