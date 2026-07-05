#include "rc_transmitter_and_receiver_pairing_reference.hpp"

namespace rc_transmitter_and_receiver_pairing_reference {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: RC transmitter and receiver pairing reference (Radio-control modeling (planes, cars, trucks, boats, drones)). Awaits wire-up.";
    return s;
}

}
