#include "test_runner.hpp"
#include "../modules/641_paper_chunker_and_rag_indexer/paper_chunker_and_rag_indexer.hpp"

namespace {

testing::TestOutcome run_paper_chunker_and_rag_indexer() {
    paper_chunker_and_rag_indexer::init();
    auto s = paper_chunker_and_rag_indexer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    paper_chunker_and_rag_indexer::shutdown();
    return testing::ok();
}

const int _reg_paper_chunker_and_rag_indexer = testing::register_test(
    "paper_chunker_and_rag_indexer",
    "641_paper_chunker_and_rag_indexer: stub status check",
    &run_paper_chunker_and_rag_indexer);

}
