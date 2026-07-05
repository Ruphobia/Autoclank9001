#include "test_runner.hpp"
#include "../modules/408_pcb_silkscreen_reader/pcb_silkscreen_reader.hpp"

namespace {

testing::TestOutcome run_pcb_silkscreen_reader() {
    pcb_silkscreen_reader::init();
    auto s = pcb_silkscreen_reader::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pcb_silkscreen_reader::shutdown();
    return testing::ok();
}

const int _reg_pcb_silkscreen_reader = testing::register_test(
    "pcb_silkscreen_reader",
    "408_pcb_silkscreen_reader: stub status check",
    &run_pcb_silkscreen_reader);

}
