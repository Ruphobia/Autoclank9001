#include "test_runner.hpp"
#include "../modules/243_stored_procedure_ide/stored_procedure_ide.hpp"

namespace {

testing::TestOutcome run_stored_procedure_ide() {
    stored_procedure_ide::init();
    auto s = stored_procedure_ide::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    stored_procedure_ide::shutdown();
    return testing::ok();
}

const int _reg_stored_procedure_ide = testing::register_test(
    "stored_procedure_ide",
    "243_stored_procedure_ide: stub status check",
    &run_stored_procedure_ide);

}
