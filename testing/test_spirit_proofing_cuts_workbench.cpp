#include "test_runner.hpp"
#include "../modules/1800_spirit_proofing_cuts_workbench/spirit_proofing_cuts_workbench.hpp"

namespace {

testing::TestOutcome run_spirit_proofing_cuts_workbench() {
    spirit_proofing_cuts_workbench::init();
    auto s = spirit_proofing_cuts_workbench::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    spirit_proofing_cuts_workbench::shutdown();
    return testing::ok();
}

const int _reg_spirit_proofing_cuts_workbench = testing::register_test(
    "spirit_proofing_cuts_workbench",
    "1800_spirit_proofing_cuts_workbench: stub status check",
    &run_spirit_proofing_cuts_workbench);

}
