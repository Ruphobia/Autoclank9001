#include "test_runner.hpp"
#include "../modules/1625_inpainter/inpainter.hpp"

namespace {

testing::TestOutcome run_inpainter() {
    inpainter::init();
    auto s = inpainter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    inpainter::shutdown();
    return testing::ok();
}

const int _reg_inpainter = testing::register_test(
    "inpainter",
    "1625_inpainter: stub status check",
    &run_inpainter);

}
