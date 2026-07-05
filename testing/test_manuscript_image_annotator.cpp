// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/717_manuscript_image_annotator/manuscript_image_annotator.hpp"

namespace {

testing::TestOutcome run_manuscript_image_annotator() {
    manuscript_image_annotator::init();
    auto s = manuscript_image_annotator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    manuscript_image_annotator::shutdown();
    return testing::ok();
}

const int _reg_manuscript_image_annotator = testing::register_test(
    "manuscript_image_annotator",
    "717_manuscript_image_annotator: stub status check",
    &run_manuscript_image_annotator);

}
