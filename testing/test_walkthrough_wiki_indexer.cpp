#include "test_runner.hpp"
#include "../modules/1978_walkthrough_wiki_indexer/walkthrough_wiki_indexer.hpp"

namespace {

testing::TestOutcome run_walkthrough_wiki_indexer() {
    walkthrough_wiki_indexer::init();
    auto s = walkthrough_wiki_indexer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    walkthrough_wiki_indexer::shutdown();
    return testing::ok();
}

const int _reg_walkthrough_wiki_indexer = testing::register_test(
    "walkthrough_wiki_indexer",
    "1978_walkthrough_wiki_indexer: stub status check",
    &run_walkthrough_wiki_indexer);

}
