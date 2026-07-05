#include "test_runner.hpp"
#include "../modules/627_embedding_pipeline/embedding_pipeline.hpp"

namespace {

testing::TestOutcome run_embedding_pipeline() {
    embedding_pipeline::init();
    auto s = embedding_pipeline::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    embedding_pipeline::shutdown();
    return testing::ok();
}

const int _reg_embedding_pipeline = testing::register_test(
    "embedding_pipeline",
    "627_embedding_pipeline: stub status check",
    &run_embedding_pipeline);

}
