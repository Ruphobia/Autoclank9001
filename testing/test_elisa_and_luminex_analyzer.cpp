#include "test_runner.hpp"
#include "../modules/1374_elisa_and_luminex_analyzer/elisa_and_luminex_analyzer.hpp"

namespace {

testing::TestOutcome run_elisa_and_luminex_analyzer() {
    elisa_and_luminex_analyzer::init();
    auto s = elisa_and_luminex_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    elisa_and_luminex_analyzer::shutdown();
    return testing::ok();
}

const int _reg_elisa_and_luminex_analyzer = testing::register_test(
    "elisa_and_luminex_analyzer",
    "1374_elisa_and_luminex_analyzer: stub status check",
    &run_elisa_and_luminex_analyzer);

}
