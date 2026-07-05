#include "test_runner.hpp"
#include "../modules/155_feed_and_sitemap_inspector/feed_and_sitemap_inspector.hpp"

namespace {

testing::TestOutcome run_feed_and_sitemap_inspector() {
    feed_and_sitemap_inspector::init();
    auto s = feed_and_sitemap_inspector::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    feed_and_sitemap_inspector::shutdown();
    return testing::ok();
}

const int _reg_feed_and_sitemap_inspector = testing::register_test(
    "feed_and_sitemap_inspector",
    "155_feed_and_sitemap_inspector: stub status check",
    &run_feed_and_sitemap_inspector);

}
