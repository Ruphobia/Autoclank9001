#include "test_runner.hpp"
#include "../modules/1070_idf_curve_generator/idf_curve_generator.hpp"

namespace {

testing::TestOutcome run_idf_curve_generator() {
    idf_curve_generator::init();
    auto s = idf_curve_generator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    idf_curve_generator::shutdown();
    return testing::ok();
}

const int _reg_idf_curve_generator = testing::register_test(
    "idf_curve_generator",
    "1070_idf_curve_generator: stub status check",
    &run_idf_curve_generator);

}
