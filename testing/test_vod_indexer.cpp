#include "test_runner.hpp"
#include "../modules/1994_vod_indexer/vod_indexer.hpp"

namespace {

testing::TestOutcome run_vod_indexer() {
    vod_indexer::init();
    auto s = vod_indexer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    vod_indexer::shutdown();
    return testing::ok();
}

const int _reg_vod_indexer = testing::register_test(
    "vod_indexer",
    "1994_vod_indexer: stub status check",
    &run_vod_indexer);

}
