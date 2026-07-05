#include "test_runner.hpp"
#include "../modules/732_concordance_and_lexicon_workbench/concordance_and_lexicon_workbench.hpp"

namespace {

testing::TestOutcome run_concordance_and_lexicon_workbench() {
    concordance_and_lexicon_workbench::init();
    auto s = concordance_and_lexicon_workbench::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    concordance_and_lexicon_workbench::shutdown();
    return testing::ok();
}

const int _reg_concordance_and_lexicon_workbench = testing::register_test(
    "concordance_and_lexicon_workbench",
    "732_concordance_and_lexicon_workbench: stub status check",
    &run_concordance_and_lexicon_workbench);

}
