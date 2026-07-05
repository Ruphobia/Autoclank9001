#include "test_runner.hpp"
#include "../modules/449_retrospective_workspace/retrospective_workspace.hpp"

namespace {

testing::TestOutcome run_retrospective_workspace() {
    retrospective_workspace::init();
    auto s = retrospective_workspace::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    retrospective_workspace::shutdown();
    return testing::ok();
}

const int _reg_retrospective_workspace = testing::register_test(
    "retrospective_workspace",
    "449_retrospective_workspace: stub status check",
    &run_retrospective_workspace);

}
