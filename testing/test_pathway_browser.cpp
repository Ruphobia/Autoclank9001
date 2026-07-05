#include "test_runner.hpp"
#include "../modules/1414_pathway_browser/pathway_browser.hpp"

namespace {

testing::TestOutcome run_pathway_browser() {
    pathway_browser::init();
    auto s = pathway_browser::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pathway_browser::shutdown();
    return testing::ok();
}

const int _reg_pathway_browser = testing::register_test(
    "pathway_browser",
    "1414_pathway_browser: stub status check",
    &run_pathway_browser);

}
