#include "test_runner.hpp"
#include "../modules/1657_prompt_library/prompt_library.hpp"

namespace {

testing::TestOutcome run_prompt_library() {
    prompt_library::init();
    auto s = prompt_library::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    prompt_library::shutdown();
    return testing::ok();
}

const int _reg_prompt_library = testing::register_test(
    "prompt_library",
    "1657_prompt_library: stub status check",
    &run_prompt_library);

}
