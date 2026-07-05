#include "test_runner.hpp"
#include "../modules/1064_sanitary_sewer_designer/sanitary_sewer_designer.hpp"

namespace {

testing::TestOutcome run_sanitary_sewer_designer() {
    sanitary_sewer_designer::init();
    auto s = sanitary_sewer_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    sanitary_sewer_designer::shutdown();
    return testing::ok();
}

const int _reg_sanitary_sewer_designer = testing::register_test(
    "sanitary_sewer_designer",
    "1064_sanitary_sewer_designer: stub status check",
    &run_sanitary_sewer_designer);

}
