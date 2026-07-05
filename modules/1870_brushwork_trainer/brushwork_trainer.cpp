#include "brushwork_trainer.hpp"

namespace brushwork_trainer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Brushwork trainer (Visual arts: calligraphy, painting, sculpture). Awaits wire-up.";
    return s;
}

}
