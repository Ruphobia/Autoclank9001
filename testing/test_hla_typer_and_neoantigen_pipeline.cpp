// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1366_hla_typer_and_neoantigen_pipeline/hla_typer_and_neoantigen_pipeline.hpp"

namespace {

testing::TestOutcome run_hla_typer_and_neoantigen_pipeline() {
    hla_typer_and_neoantigen_pipeline::init();
    auto s = hla_typer_and_neoantigen_pipeline::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    hla_typer_and_neoantigen_pipeline::shutdown();
    return testing::ok();
}

const int _reg_hla_typer_and_neoantigen_pipeline = testing::register_test(
    "hla_typer_and_neoantigen_pipeline",
    "1366_hla_typer_and_neoantigen_pipeline: stub status check",
    &run_hla_typer_and_neoantigen_pipeline);

}
