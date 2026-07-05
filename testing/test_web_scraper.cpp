#include "test_runner.hpp"
#include "../modules/145_web_scraper/web_scraper.hpp"

namespace {

testing::TestOutcome run_web_scraper() {
    web_scraper::init();
    auto s = web_scraper::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    web_scraper::shutdown();
    return testing::ok();
}

const int _reg_web_scraper = testing::register_test(
    "web_scraper",
    "145_web_scraper: stub status check",
    &run_web_scraper);

}
