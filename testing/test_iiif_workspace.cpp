#include "test_runner.hpp"
#include "../modules/689_iiif_workspace/iiif_workspace.hpp"

namespace {

testing::TestOutcome run_iiif_workspace() {
    iiif_workspace::init();
    auto s = iiif_workspace::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    iiif_workspace::shutdown();
    return testing::ok();
}

const int _reg_iiif_workspace = testing::register_test(
    "iiif_workspace",
    "689_iiif_workspace: stub status check",
    &run_iiif_workspace);

}
