#include "test_runner.hpp"
#include "../modules/740_exegesis_notebook/exegesis_notebook.hpp"

namespace {

testing::TestOutcome run_exegesis_notebook() {
    exegesis_notebook::init();
    auto s = exegesis_notebook::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    exegesis_notebook::shutdown();
    return testing::ok();
}

const int _reg_exegesis_notebook = testing::register_test(
    "exegesis_notebook",
    "740_exegesis_notebook: stub status check",
    &run_exegesis_notebook);

}
