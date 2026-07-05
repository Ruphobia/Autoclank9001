#include "test_runner.hpp"
#include "../modules/635_flashcard_generator/flashcard_generator.hpp"

namespace {

testing::TestOutcome run_flashcard_generator() {
    flashcard_generator::init();
    auto s = flashcard_generator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    flashcard_generator::shutdown();
    return testing::ok();
}

const int _reg_flashcard_generator = testing::register_test(
    "flashcard_generator",
    "635_flashcard_generator: stub status check",
    &run_flashcard_generator);

}
