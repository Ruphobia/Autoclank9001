#include "test_runner.hpp"
#include "../modules/671_embedding_atlas/embedding_atlas.hpp"

namespace {

testing::TestOutcome run_embedding_atlas() {
    embedding_atlas::init();
    auto s = embedding_atlas::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    embedding_atlas::shutdown();
    return testing::ok();
}

const int _reg_embedding_atlas = testing::register_test(
    "embedding_atlas",
    "671_embedding_atlas: stub status check",
    &run_embedding_atlas);

}
