#include "test_runner.hpp"
#include "../modules/834_morphology_and_lemma_analyzer/morphology_and_lemma_analyzer.hpp"

namespace {

testing::TestOutcome run_morphology_and_lemma_analyzer() {
    morphology_and_lemma_analyzer::init();
    auto s = morphology_and_lemma_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    morphology_and_lemma_analyzer::shutdown();
    return testing::ok();
}

const int _reg_morphology_and_lemma_analyzer = testing::register_test(
    "morphology_and_lemma_analyzer",
    "834_morphology_and_lemma_analyzer: stub status check",
    &run_morphology_and_lemma_analyzer);

}
