// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1870_brushwork_trainer/brushwork_trainer.hpp"

namespace {

testing::TestOutcome run_brushwork_trainer() {
    brushwork_trainer::init();
    auto s = brushwork_trainer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    brushwork_trainer::shutdown();
    return testing::ok();
}

const int _reg_brushwork_trainer = testing::register_test(
    "brushwork_trainer",
    "1870_brushwork_trainer: stub status check",
    &run_brushwork_trainer);

}
