#include "test_runner.hpp"
#include "../modules/1356_strain_typer/strain_typer.hpp"

namespace {

testing::TestOutcome run_strain_typer() {
    strain_typer::init();
    auto s = strain_typer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    strain_typer::shutdown();
    return testing::ok();
}

const int _reg_strain_typer = testing::register_test(
    "strain_typer",
    "1356_strain_typer: stub status check",
    &run_strain_typer);

}
