#include "test_runner.hpp"
#include "../modules/082_literature_review_synthesizer/literature_review_synthesizer.hpp"

namespace {

testing::TestOutcome run_literature_review_synthesizer() {
    literature_review_synthesizer::init();
    auto s = literature_review_synthesizer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    literature_review_synthesizer::shutdown();
    return testing::ok();
}

const int _reg_literature_review_synthesizer = testing::register_test(
    "literature_review_synthesizer",
    "082_literature_review_synthesizer: stub status check",
    &run_literature_review_synthesizer);

}
