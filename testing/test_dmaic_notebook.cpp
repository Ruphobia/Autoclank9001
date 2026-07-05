#include "test_runner.hpp"
#include "../modules/464_dmaic_notebook/dmaic_notebook.hpp"

namespace {

testing::TestOutcome run_dmaic_notebook() {
    dmaic_notebook::init();
    auto s = dmaic_notebook::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    dmaic_notebook::shutdown();
    return testing::ok();
}

const int _reg_dmaic_notebook = testing::register_test(
    "dmaic_notebook",
    "464_dmaic_notebook: stub status check",
    &run_dmaic_notebook);

}
