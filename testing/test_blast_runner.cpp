#include "test_runner.hpp"
#include "../modules/1348_blast_runner/blast_runner.hpp"

namespace {

testing::TestOutcome run_blast_runner() {
    blast_runner::init();
    auto s = blast_runner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    blast_runner::shutdown();
    return testing::ok();
}

const int _reg_blast_runner = testing::register_test(
    "blast_runner",
    "1348_blast_runner: stub status check",
    &run_blast_runner);

}
