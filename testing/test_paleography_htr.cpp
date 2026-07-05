#include "test_runner.hpp"
#include "../modules/686_paleography_htr/paleography_htr.hpp"

namespace {

testing::TestOutcome run_paleography_htr() {
    paleography_htr::init();
    auto s = paleography_htr::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    paleography_htr::shutdown();
    return testing::ok();
}

const int _reg_paleography_htr = testing::register_test(
    "paleography_htr",
    "686_paleography_htr: stub status check",
    &run_paleography_htr);

}
