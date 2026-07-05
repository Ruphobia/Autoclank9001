#include "test_runner.hpp"
#include "../modules/1974_meta_snapshot_scraper/meta_snapshot_scraper.hpp"

namespace {

testing::TestOutcome run_meta_snapshot_scraper() {
    meta_snapshot_scraper::init();
    auto s = meta_snapshot_scraper::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    meta_snapshot_scraper::shutdown();
    return testing::ok();
}

const int _reg_meta_snapshot_scraper = testing::register_test(
    "meta_snapshot_scraper",
    "1974_meta_snapshot_scraper: stub status check",
    &run_meta_snapshot_scraper);

}
