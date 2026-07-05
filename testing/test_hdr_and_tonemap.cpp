#include "test_runner.hpp"
#include "../modules/1636_hdr_and_tonemap/hdr_and_tonemap.hpp"

namespace {

testing::TestOutcome run_hdr_and_tonemap() {
    hdr_and_tonemap::init();
    auto s = hdr_and_tonemap::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    hdr_and_tonemap::shutdown();
    return testing::ok();
}

const int _reg_hdr_and_tonemap = testing::register_test(
    "hdr_and_tonemap",
    "1636_hdr_and_tonemap: stub status check",
    &run_hdr_and_tonemap);

}
