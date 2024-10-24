// Author: Jake Rieger
// Created: 10/24/2024.
//

#include "XnPak.h"

int main(int argc, char* argv[]) {
    const Xen::Manifest manifest("Example/Content.manifest");
    std::cout << manifest.toString() << '\n';
    for (auto& asset : manifest.assets) {
        std::cout << asset.toString() << '\n';
    }

    return 0;
}