#include "test_runner.hpp"
#include "../modules/837_back_translation_qa/back_translation_qa.hpp"

namespace {

testing::TestOutcome run_back_translation_qa() {
    back_translation_qa::init();
    auto s = back_translation_qa::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    back_translation_qa::shutdown();
    return testing::ok();
}

const int _reg_back_translation_qa = testing::register_test(
    "back_translation_qa",
    "837_back_translation_qa: stub status check",
    &run_back_translation_qa);

}
