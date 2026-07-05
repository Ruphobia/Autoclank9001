#include "test_runner.hpp"
#include "../modules/1927_paperwork_generator/paperwork_generator.hpp"

namespace {

testing::TestOutcome run_paperwork_generator() {
    paperwork_generator::init();
    auto s = paperwork_generator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    paperwork_generator::shutdown();
    return testing::ok();
}

const int _reg_paperwork_generator = testing::register_test(
    "paperwork_generator",
    "1927_paperwork_generator: stub status check",
    &run_paperwork_generator);

}
