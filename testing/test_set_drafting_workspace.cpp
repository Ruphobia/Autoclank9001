#include "test_runner.hpp"
#include "../modules/1921_set_drafting_workspace/set_drafting_workspace.hpp"

namespace {

testing::TestOutcome run_set_drafting_workspace() {
    set_drafting_workspace::init();
    auto s = set_drafting_workspace::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    set_drafting_workspace::shutdown();
    return testing::ok();
}

const int _reg_set_drafting_workspace = testing::register_test(
    "set_drafting_workspace",
    "1921_set_drafting_workspace: stub status check",
    &run_set_drafting_workspace);

}
