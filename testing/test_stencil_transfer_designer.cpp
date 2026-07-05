#include "test_runner.hpp"
#include "../modules/1896_stencil_transfer_designer/stencil_transfer_designer.hpp"

namespace {

testing::TestOutcome run_stencil_transfer_designer() {
    stencil_transfer_designer::init();
    auto s = stencil_transfer_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    stencil_transfer_designer::shutdown();
    return testing::ok();
}

const int _reg_stencil_transfer_designer = testing::register_test(
    "stencil_transfer_designer",
    "1896_stencil_transfer_designer: stub status check",
    &run_stencil_transfer_designer);

}
