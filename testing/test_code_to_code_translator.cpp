#include "test_runner.hpp"
#include "../modules/103_code_to_code_translator/code_to_code_translator.hpp"

namespace {

testing::TestOutcome run_code_to_code_translator() {
    code_to_code_translator::init();
    auto s = code_to_code_translator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    code_to_code_translator::shutdown();
    return testing::ok();
}

const int _reg_code_to_code_translator = testing::register_test(
    "code_to_code_translator",
    "103_code_to_code_translator: stub status check",
    &run_code_to_code_translator);

}
