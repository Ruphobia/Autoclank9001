#include "test_runner.hpp"
#include "../modules/678_prompt_lab/prompt_lab.hpp"

namespace {

testing::TestOutcome run_prompt_lab() {
    prompt_lab::init();
    auto s = prompt_lab::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    prompt_lab::shutdown();
    return testing::ok();
}

const int _reg_prompt_lab = testing::register_test(
    "prompt_lab",
    "678_prompt_lab: stub status check",
    &run_prompt_lab);

}
