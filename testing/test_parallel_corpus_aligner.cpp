// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/713_parallel_corpus_aligner/parallel_corpus_aligner.hpp"

namespace {

testing::TestOutcome run_parallel_corpus_aligner() {
    parallel_corpus_aligner::init();
    auto s = parallel_corpus_aligner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    parallel_corpus_aligner::shutdown();
    return testing::ok();
}

const int _reg_parallel_corpus_aligner = testing::register_test(
    "parallel_corpus_aligner",
    "713_parallel_corpus_aligner: stub status check",
    &run_parallel_corpus_aligner);

}
