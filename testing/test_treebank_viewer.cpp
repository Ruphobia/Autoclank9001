#include "test_runner.hpp"
#include "../modules/801_treebank_viewer/treebank_viewer.hpp"

namespace {

testing::TestOutcome run_treebank_viewer() {
    treebank_viewer::init();
    auto s = treebank_viewer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    treebank_viewer::shutdown();
    return testing::ok();
}

const int _reg_treebank_viewer = testing::register_test(
    "treebank_viewer",
    "801_treebank_viewer: stub status check",
    &run_treebank_viewer);

}
