#include "test_runner.hpp"
#include "../modules/1007_tempering_color_and_spark_test_guide/tempering_color_and_spark_test_guide.hpp"

namespace {

testing::TestOutcome run_tempering_color_and_spark_test_guide() {
    tempering_color_and_spark_test_guide::init();
    auto s = tempering_color_and_spark_test_guide::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    tempering_color_and_spark_test_guide::shutdown();
    return testing::ok();
}

const int _reg_tempering_color_and_spark_test_guide = testing::register_test(
    "tempering_color_and_spark_test_guide",
    "1007_tempering_color_and_spark_test_guide: stub status check",
    &run_tempering_color_and_spark_test_guide);

}
