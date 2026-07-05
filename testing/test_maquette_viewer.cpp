#include "test_runner.hpp"
#include "../modules/1876_maquette_viewer/maquette_viewer.hpp"

namespace {

testing::TestOutcome run_maquette_viewer() {
    maquette_viewer::init();
    auto s = maquette_viewer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    maquette_viewer::shutdown();
    return testing::ok();
}

const int _reg_maquette_viewer = testing::register_test(
    "maquette_viewer",
    "1876_maquette_viewer: stub status check",
    &run_maquette_viewer);

}
