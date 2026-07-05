#include "test_runner.hpp"
#include "../modules/1344_bioinformatics_format_converter/bioinformatics_format_converter.hpp"

namespace {

testing::TestOutcome run_bioinformatics_format_converter() {
    bioinformatics_format_converter::init();
    auto s = bioinformatics_format_converter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    bioinformatics_format_converter::shutdown();
    return testing::ok();
}

const int _reg_bioinformatics_format_converter = testing::register_test(
    "bioinformatics_format_converter",
    "1344_bioinformatics_format_converter: stub status check",
    &run_bioinformatics_format_converter);

}
