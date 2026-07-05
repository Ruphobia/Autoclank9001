#include "test_runner.hpp"
#include "../modules/851_filter_designer/filter_designer.hpp"

namespace {

testing::TestOutcome run_filter_designer() {
    filter_designer::init();
    auto s = filter_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    filter_designer::shutdown();
    return testing::ok();
}

const int _reg_filter_designer = testing::register_test(
    "filter_designer",
    "851_filter_designer: stub status check",
    &run_filter_designer);

}
