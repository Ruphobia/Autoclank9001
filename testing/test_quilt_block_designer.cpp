#include "test_runner.hpp"
#include "../modules/1775_quilt_block_designer/quilt_block_designer.hpp"

namespace {

testing::TestOutcome run_quilt_block_designer() {
    quilt_block_designer::init();
    auto s = quilt_block_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    quilt_block_designer::shutdown();
    return testing::ok();
}

const int _reg_quilt_block_designer = testing::register_test(
    "quilt_block_designer",
    "1775_quilt_block_designer: stub status check",
    &run_quilt_block_designer);

}
