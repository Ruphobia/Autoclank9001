#include "test_runner.hpp"
#include "../modules/097_notebook_runner/notebook_runner.hpp"

namespace {

testing::TestOutcome run_notebook_runner() {
    notebook_runner::init();
    auto s = notebook_runner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    notebook_runner::shutdown();
    return testing::ok();
}

const int _reg_notebook_runner = testing::register_test(
    "notebook_runner",
    "097_notebook_runner: stub status check",
    &run_notebook_runner);

}
