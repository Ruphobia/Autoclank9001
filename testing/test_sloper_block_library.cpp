#include "test_runner.hpp"
#include "../modules/1764_sloper_block_library/sloper_block_library.hpp"

namespace {

testing::TestOutcome run_sloper_block_library() {
    sloper_block_library::init();
    auto s = sloper_block_library::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    sloper_block_library::shutdown();
    return testing::ok();
}

const int _reg_sloper_block_library = testing::register_test(
    "sloper_block_library",
    "1764_sloper_block_library: stub status check",
    &run_sloper_block_library);

}
