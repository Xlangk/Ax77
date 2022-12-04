#include "Linux/ConsoleWindow.h"
#include "Ax256/Page.h"

int main() {
    using namespace Ax256;

    Linux::ConsoleWindow console(1023, 1023);
    Page ram(256);

    auto buf = console.getGfxBuffer();

    int width = console.width();
    int height = console.height();

    // Test graphical rendering latency.
    for (int x = 0; x < width * height; x++) {
        if (buf[x] == 0x101010) {
            buf[x] = 0xFF0000;
        }
    }

    usleep(100000);

    for (int x = 0; x < width * height; x++) {
        if (buf[x] == 0xFF0000) {
            buf[x] = 0x000000;
        }
    }

    usleep(100000);
    console.setGraphicsColor(0x000000);

    usleep(100000);
    console.generateGrid(0x202020, 0x202020, 40, 20, 80, 40, 0, 0);
    console.setGraphicsColor(0x000000);

    // End of test.

    std::string text = "Hello World";
    int charsVisible = 1;

    int line = 1;
    int color = 0xFFFFFF;

    auto clearText = [&] () {
        console.placeText(10, line * 10, 7, 0x000000, text);
    };

    auto writeText = [&] () {
        while (charsVisible <= text.length()) {
            clearText();
            console.placeText(10, line * 10, 7, color, text.substr(0, charsVisible));
            charsVisible++;
            usleep(10000);
        }
    };

    writeText();
    text = "This is a test... Please wait...";
    charsVisible = 1;
    line = 2;
    writeText();
    console.generateGrid(0x202020, 0x202020, 40, 20, 80, 40, 0, 0);
    text = "This is a test... Please wait... Done.";
    charsVisible = 1;
    line = 3;
    writeText();
    usleep(1000000);
    console.setGraphicsColor(0xFFFFFF);
    console.generateGrid(0x202020, 0x202020, 80, 40, 80, 40, 0, 0);
    line = 1;
    charsVisible = 1;
    text = "Inverse";
    color = 0x000000;
    writeText();
    console.setGraphicsColor(0x000000);
    line = 1;
    charsVisible = 1;
    text = "Inverse";
    color = 0xFFFFFF;
    writeText();

    console.setGraphicsColor(0x000000);

    int times = 0;
    int max = 10;

    while (times < max) {
        if (times % 2 == 0) {
            console.placeText(1, 1, 7, 0xFFFFFF, "-");
        } else {
            console.placeText(1, 1, 7, 0x000000, "-");
        }

        times++;
        usleep(500000);
    }

    console.placeText(10, 10, 7, 0xFFFFFF, "Bootloader Kernel will boot, NOW!");

    console.block();
    return 0;
}