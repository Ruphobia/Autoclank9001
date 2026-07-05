#include "test_runner.hpp"
#include "../modules/833_corpus_and_concordance_browser/corpus_and_concordance_browser.hpp"

namespace {

testing::TestOutcome run_corpus_and_concordance_browser() {
    corpus_and_concordance_browser::init();
    auto s = corpus_and_concordance_browser::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    corpus_and_concordance_browser::shutdown();
    return testing::ok();
}

const int _reg_corpus_and_concordance_browser = testing::register_test(
    "corpus_and_concordance_browser",
    "833_corpus_and_concordance_browser: stub status check",
    &run_corpus_and_concordance_browser);

}
