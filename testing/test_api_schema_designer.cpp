#include "test_runner.hpp"
#include "../modules/122_api_schema_designer/api_schema_designer.hpp"

namespace {

testing::TestOutcome run_api_schema_designer() {
    api_schema_designer::init();
    auto s = api_schema_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    api_schema_designer::shutdown();
    return testing::ok();
}

const int _reg_api_schema_designer = testing::register_test(
    "api_schema_designer",
    "122_api_schema_designer: stub status check",
    &run_api_schema_designer);

}
