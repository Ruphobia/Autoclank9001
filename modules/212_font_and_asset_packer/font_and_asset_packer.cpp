#include "font_and_asset_packer.hpp"

namespace font_and_asset_packer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Font and asset packer (Firmware and embedded). Awaits wire-up.";
    return s;
}

}
