#include "test_runner.hpp"
#include "../modules/2186_dream_and_imagery_journal/dream_and_imagery_journal.hpp"

namespace {

testing::TestOutcome run_dream_and_imagery_journal() {
    dream_and_imagery_journal::init();
    auto s = dream_and_imagery_journal::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    dream_and_imagery_journal::shutdown();
    return testing::ok();
}

const int _reg_dream_and_imagery_journal = testing::register_test(
    "dream_and_imagery_journal",
    "2186_dream_and_imagery_journal: stub status check",
    &run_dream_and_imagery_journal);

}
