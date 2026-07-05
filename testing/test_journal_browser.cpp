#include "test_runner.hpp"
#include "../modules/258_journal_browser/journal_browser.hpp"

namespace {

testing::TestOutcome run_journal_browser() {
    journal_browser::init();
    auto s = journal_browser::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    journal_browser::shutdown();
    return testing::ok();
}

const int _reg_journal_browser = testing::register_test(
    "journal_browser",
    "258_journal_browser: stub status check",
    &run_journal_browser);

}
