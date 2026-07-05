#include "test_runner.hpp"
#include "../modules/790_corpus_concordancer/corpus_concordancer.hpp"

namespace {

testing::TestOutcome run_corpus_concordancer() {
    corpus_concordancer::init();
    auto s = corpus_concordancer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    corpus_concordancer::shutdown();
    return testing::ok();
}

const int _reg_corpus_concordancer = testing::register_test(
    "corpus_concordancer",
    "790_corpus_concordancer: stub status check",
    &run_corpus_concordancer);

}
