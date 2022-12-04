#include "Linux/ConsoleWindow.h"

int main() {
    using namespace Ax256;

    Linux::ConsoleWindow console;

    auto buf = console.getGfxBuffer();

    // Test graphical rendering latency.
    for (int x = 0; x < 500 * 400; x++) {
        if (buf[x] == 0x101010) {
            buf[x] = 0xFF0000;
        }
    }

    usleep(100000);

    for (int x = 0; x < 500 * 400; x++) {
        if (buf[x] == 0xFF0000) {
            buf[x] = 0x000000;
        }
    }

    usleep(100000);
    console.setGraphicsColor(0x000000);

    usleep(100000);
    console.generateGrid(0x202020, 0x202020, 3, 3, 10, 10, 5, 5);
    console.setGraphicsColor(0x000000);

    // End of test.

    console.block();
    return 0;
}