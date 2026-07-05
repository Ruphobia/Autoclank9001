#include "test_runner.hpp"
#include "../modules/577_historical_patent_browser/historical_patent_browser.hpp"

namespace {

testing::TestOutcome run_historical_patent_browser() {
    historical_patent_browser::init();
    auto s = historical_patent_browser::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    historical_patent_browser::shutdown();
    return testing::ok();
}

const int _reg_historical_patent_browser = testing::register_test(
    "historical_patent_browser",
    "577_historical_patent_browser: stub status check",
    &run_historical_patent_browser);

}
