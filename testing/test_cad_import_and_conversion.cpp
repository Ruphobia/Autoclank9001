#include "test_runner.hpp"
#include "../modules/1255_cad_import_and_conversion/cad_import_and_conversion.hpp"

namespace {

testing::TestOutcome run_cad_import_and_conversion() {
    cad_import_and_conversion::init();
    auto s = cad_import_and_conversion::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    cad_import_and_conversion::shutdown();
    return testing::ok();
}

const int _reg_cad_import_and_conversion = testing::register_test(
    "cad_import_and_conversion",
    "1255_cad_import_and_conversion: stub status check",
    &run_cad_import_and_conversion);

}
