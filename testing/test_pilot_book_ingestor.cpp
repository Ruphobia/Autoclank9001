// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2112_pilot_book_ingestor/pilot_book_ingestor.hpp"

namespace {

testing::TestOutcome run_pilot_book_ingestor() {
    pilot_book_ingestor::init();
    auto s = pilot_book_ingestor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pilot_book_ingestor::shutdown();
    return testing::ok();
}

const int _reg_pilot_book_ingestor = testing::register_test(
    "pilot_book_ingestor",
    "2112_pilot_book_ingestor: stub status check",
    &run_pilot_book_ingestor);

}
