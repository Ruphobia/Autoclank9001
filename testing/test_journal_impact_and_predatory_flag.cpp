#include "test_runner.hpp"
#include "../modules/646_journal_impact_and_predatory_flag/journal_impact_and_predatory_flag.hpp"

namespace {

testing::TestOutcome run_journal_impact_and_predatory_flag() {
    journal_impact_and_predatory_flag::init();
    auto s = journal_impact_and_predatory_flag::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    journal_impact_and_predatory_flag::shutdown();
    return testing::ok();
}

const int _reg_journal_impact_and_predatory_flag = testing::register_test(
    "journal_impact_and_predatory_flag",
    "646_journal_impact_and_predatory_flag: stub status check",
    &run_journal_impact_and_predatory_flag);

}
