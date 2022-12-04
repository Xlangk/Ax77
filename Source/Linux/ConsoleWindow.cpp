#include <atomic>
#include <X11/Xutil.h>
#include "ConsoleWindow.h"

namespace Ax256::Linux {
    ConsoleWindow::ConsoleWindow(int w, int h) : m_enable(true), m_padding(10), m_gfxHeight(h), m_gfxWidth(w),
                                                 m_cWidth(800), m_cHeight(400) {
        setWindowState(true);
        // Create the window.
        m_display = XOpenDisplay(nullptr);
        if (m_display == nullptr) {
            fprintf(stderr, "Cannot open display\n");
            exit(1);
        }

        int screen = DefaultScreen(m_display);

        // Create 2 independent windows, first one is for the console serial output, second one is for the graphics output.
        m_window = XCreateSimpleWindow(m_display, RootWindow(m_display, screen), 0, 0, m_cWidth, m_cHeight, 1,
                                       BlackPixel(m_display, screen), BlackPixel(m_display, screen));
        m_graphicsContent = XCreateSimpleWindow(m_display, RootWindow(m_display, screen), 0, 0, m_gfxWidth, m_gfxHeight,
                                                1,
                                                BlackPixel(m_display, screen), BlackPixel(m_display, screen));

        // Set the window title.
        XStoreName(m_display, m_window, "Ax256 Serial Buffer");
        XStoreName(m_display, m_graphicsContent, "Ax256 Graphics Buffer");

        // Set the window to be visible.
        XMapWindow(m_display, m_window);
        XMapWindow(m_display, m_graphicsContent);

        // Move windows.
        XMoveWindow(m_display, m_window, m_padding, m_padding);
        XMoveWindow(m_display, m_graphicsContent, m_cWidth + m_padding + m_padding, m_padding);

        XFlush(m_display);

        // Initialize graphics buffer with grey pixels for testing.
        m_gfxBuffer = new int[m_gfxWidth * m_gfxHeight];

        m_thread = new std::thread([this]() {
            while (m_enable) {
                usleep(1);

                // Write graphics buffer for graphics window.
                for (int i = 0; i < m_gfxWidth * m_gfxHeight; i++) {
                    XSetForeground(m_display, DefaultGC(m_display, 0), m_gfxBuffer[i]);
                    XDrawPoint(m_display, m_graphicsContent, DefaultGC(m_display, 0), i % m_gfxWidth, i / m_gfxWidth);
                }
            }
        });

        for (int i = 0; i < m_gfxWidth * m_gfxHeight; i++) {
            m_gfxBuffer[i] = 0x101010;
            if (i % 10 == 0) {
                // set the pixel to red
                m_gfxBuffer[i] = 0xFF0000;
            }

            if (i % 20 == 0) {
                m_gfxBuffer[i] = 0x00FF00;
            }

            if (i % 30 == 0) {
                m_gfxBuffer[i] = 0x0000FF;
            }

            if (i % 400 == 0) {
                usleep(1);
            }
        }

        generateGrid(0x50FFAB, 0x50FFAB, 1, 1, 90, 90, 0, 0);
    }

    bool ConsoleWindow::setWindowState(bool enable) {
        if (enable == m_enable) return true;
        if (enable) {
            m_enable = true;
            return true;
        }
        m_enable = false;
        return true;
    }

    bool ConsoleWindow::clearGfx() {
        if (!m_enable) return false;
        for (int i = 0; i < m_gfxWidth * m_gfxHeight; i++)
            m_gfxBuffer[i] = 0x000000;
        return true;
    }

    bool ConsoleWindow::setPixel(int x, int y, int color) {
        if (!m_enable) return false;
        if (x < 0 || x >= m_gfxWidth || y < 0 || y >= m_gfxHeight) return false;
        m_gfxBuffer[x + y * m_gfxWidth] = color;
        return true;
    }

    bool ConsoleWindow::setGraphicsColor(int color) {
        if (!m_enable) return false;
        for (int i = 0; i < m_gfxWidth * m_gfxHeight; i++)
            m_gfxBuffer[i] = color;
        return true;
    }

    void ConsoleWindow::block() {
        m_thread->join();
    }

    void ConsoleWindow::generateGrid(int colorX, int colorY, int dashLengthAndSpacingX, int dashLengthAndSpacingY,
                                     int divisionX, int divisionY, int delayX, int delayY) {
        for (int x = 0; x < m_gfxWidth; x++) {
            for (int y = 0; y < m_gfxHeight; y++) {
                if (x % divisionX == 0) {
                    if (y % dashLengthAndSpacingY == 0) {
                        m_gfxBuffer[x + y * m_gfxWidth] = colorX;
                    }
                }
                if (y % divisionY == 0) {
                    if (x % dashLengthAndSpacingX == 0) {
                        m_gfxBuffer[x + y * m_gfxWidth] = colorY;
                    }
                    usleep(delayY);
                }
            }
        }
    }

    int ConsoleWindow::width() {
        return m_gfxWidth;
    }

    int ConsoleWindow::height() {
        return m_gfxHeight;
    }

    const bool letterA[7][7] = {
            {0, 0, 0, 1, 0, 0, 0},
            {0, 0, 1, 0, 1, 0, 0},
            {0, 1, 0, 0, 0, 1, 0},
            {1, 0, 0, 0, 0, 0, 1},
            {1, 1, 1, 1, 1, 1, 1},
            {1, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 1}
    };

    const bool letterB[7][7] = {
            {1, 1, 1, 1, 1, 1, 0},
            {1, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 1},
            {1, 1, 1, 1, 1, 1, 0},
            {1, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 1},
            {1, 1, 1, 1, 1, 1, 0}
    };

    const bool letterC[7][7] = {
            {0, 1, 1, 1, 1, 1, 0},
            {1, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0},
            {1, 0, 0, 0, 0, 0, 0},
            {1, 0, 0, 0, 0, 0, 0},
            {1, 0, 0, 0, 0, 0, 1},
            {0, 1, 1, 1, 1, 1, 0}
    };

    const bool letterD[7][7] = {
            {1, 1, 1, 1, 1, 1, 0},
            {1, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 1},
            {1, 1, 1, 1, 1, 1, 0}
    };

    const bool letterE[7][7] = {
            {1, 1, 1, 1, 1, 1, 1},
            {1, 0, 0, 0, 0, 0, 0},
            {1, 0, 0, 0, 0, 0, 0},
            {1, 1, 1, 1, 1, 1, 0},
            {1, 0, 0, 0, 0, 0, 0},
            {1, 0, 0, 0, 0, 0, 0},
            {1, 1, 1, 1, 1, 1, 1}
    };

    const bool letterF[7][7] = {
            {1, 1, 1, 1, 1, 1, 1},
            {1, 0, 0, 0, 0, 0, 0},
            {1, 0, 0, 0, 0, 0, 0},
            {1, 1, 1, 1, 1, 1, 0},
            {1, 0, 0, 0, 0, 0, 0},
            {1, 0, 0, 0, 0, 0, 0},
            {1, 0, 0, 0, 0, 0, 0}
    };

    const bool letterG[7][7] = {
            {0, 1, 1, 1, 1, 1, 0},
            {1, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0},
            {1, 0, 0, 0, 0, 1, 0},
            {1, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 1},
            {0, 1, 1, 1, 1, 1, 0}
    };

    const bool letterH[7][7] = {
            {1, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 1},
            {1, 1, 1, 1, 1, 1, 1},
            {1, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 1}
    };

    const bool letterI[7][7] = {
            {0, 0, 1, 1, 1, 0, 0},
            {0, 0, 0, 1, 0, 0, 0},
            {0, 0, 0, 1, 0, 0, 0},
            {0, 0, 0, 1, 0, 0, 0},
            {0, 0, 0, 1, 0, 0, 0},
            {0, 0, 0, 1, 0, 0, 0},
            {0, 0, 1, 1, 1, 0, 0}
    };

    const bool letterJ[7][7] = {
            {0, 0, 0, 1, 1, 1, 0},
            {0, 0, 0, 0, 1, 0, 0},
            {0, 0, 0, 0, 1, 0, 0},
            {0, 0, 0, 0, 1, 0, 0},
            {1, 0, 0, 0, 1, 0, 0},
            {1, 0, 0, 0, 1, 0, 0},
            {0, 1, 1, 1, 0, 0, 0}
    };

    const bool letterK[7][7] = {
            {1, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 1, 0},
            {1, 0, 0, 0, 1, 0, 0},
            {1, 1, 1, 1, 0, 0, 0},
            {1, 0, 0, 0, 1, 0, 0},
            {1, 0, 0, 0, 0, 1, 0},
            {1, 0, 0, 0, 0, 0, 1}
    };

    const bool letterL[7][7] = {
            {1, 0, 0, 0, 0, 0, 0},
            {1, 0, 0, 0, 0, 0, 0},
            {1, 0, 0, 0, 0, 0, 0},
            {1, 0, 0, 0, 0, 0, 0},
            {1, 0, 0, 0, 0, 0, 0},
            {1, 0, 0, 0, 0, 0, 0},
            {1, 1, 1, 1, 1, 1, 1}
    };

    const bool letterM[7][7] = {
            {1, 0, 0, 0, 0, 0, 1},
            {1, 1, 0, 0, 0, 1, 1},
            {1, 0, 1, 0, 1, 0, 1},
            {1, 0, 0, 1, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 1}
    };

    const bool letterN[7][7] = {
            {1, 0, 0, 0, 0, 0, 1},
            {1, 1, 0, 0, 0, 0, 1},
            {1, 0, 1, 0, 0, 0, 1},
            {1, 0, 0, 1, 0, 0, 1},
            {1, 0, 0, 0, 1, 0, 1},
            {1, 0, 0, 0, 0, 1, 1},
            {1, 0, 0, 0, 0, 0, 1}
    };

    const bool letterO[7][7] = {
            {0, 1, 1, 1, 1, 1, 0},
            {1, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 1},
            {0, 1, 1, 1, 1, 1, 0}
    };

    const bool letterP[7][7] = {
            {1, 1, 1, 1, 1, 1, 0},
            {1, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 1},
            {1, 1, 1, 1, 1, 1, 0},
            {1, 0, 0, 0, 0, 0, 0},
            {1, 0, 0, 0, 0, 0, 0},
            {1, 0, 0, 0, 0, 0, 0}
    };

    const bool letterQ[7][7] = {
            {0, 1, 1, 1, 1, 1, 0},
            {1, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 1, 0, 0, 1},
            {1, 0, 0, 0, 1, 0, 1},
            {0, 1, 1, 1, 0, 1, 1}
    };

    const bool letterR[7][7] = {
            {1, 1, 1, 1, 1, 1, 0},
            {1, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 1},
            {1, 1, 1, 1, 1, 1, 0},
            {1, 0, 0, 0, 0, 1, 0},
            {1, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 1}
    };

    const bool letterS[7][7] = {
            {0, 1, 1, 1, 1, 1, 0},
            {1, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0},
            {0, 1, 1, 1, 1, 1, 0},
            {0, 0, 0, 0, 0, 0, 1},
            {0, 0, 0, 0, 0, 0, 1},
            {1, 1, 1, 1, 1, 1, 0}
    };

    const bool letterT[7][7] = {
            {1, 1, 1, 1, 1, 1, 1},
            {0, 0, 0, 1, 0, 0, 0},
            {0, 0, 0, 1, 0, 0, 0},
            {0, 0, 0, 1, 0, 0, 0},
            {0, 0, 0, 1, 0, 0, 0},
            {0, 0, 0, 1, 0, 0, 0},
            {0, 0, 0, 1, 0, 0, 0}
    };

    const bool letterU[7][7] = {
            {1, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 1},
            {0, 1, 1, 1, 1, 1, 0}
    };

    const bool letterV[7][7] = {
            {1, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 1},
            {0, 1, 0, 0, 0, 1, 0},
            {0, 0, 1, 0, 1, 0, 0},
            {0, 0, 0, 1, 0, 0, 0}
    };

    const bool letterW[7][7] = {
            {1, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 1},
            {1, 0, 1, 0, 1, 0, 1},
            {1, 0, 1, 0, 1, 0, 1},
            {1, 0, 0, 1, 0, 0, 1},
            {0, 1, 0, 1, 0, 1, 0}
    };

    const bool letterX[7][7] = {
            {1, 0, 0, 0, 0, 0, 1},
            {0, 1, 0, 0, 0, 1, 0},
            {0, 0, 1, 0, 1, 0, 0},
            {0, 0, 0, 1, 0, 0, 0},
            {0, 0, 1, 0, 1, 0, 0},
            {0, 1, 0, 0, 0, 1, 0},
            {1, 0, 0, 0, 0, 0, 1}
    };

    const bool letterY[7][7] = {
            {1, 0, 0, 0, 0, 0, 1},
            {0, 1, 0, 0, 0, 1, 0},
            {0, 0, 1, 0, 1, 0, 0},
            {0, 0, 0, 1, 0, 0, 0},
            {0, 0, 0, 1, 0, 0, 0},
            {0, 0, 0, 1, 0, 0, 0},
            {0, 0, 0, 1, 0, 0, 0}
    };

    const bool letterZ[7][7] = {
            {1, 1, 1, 1, 1, 1, 1},
            {0, 0, 0, 0, 0, 1, 0},
            {0, 0, 0, 0, 1, 0, 0},
            {0, 0, 0, 1, 0, 0, 0},
            {0, 0, 1, 0, 0, 0, 0},
            {0, 1, 0, 0, 0, 0, 0},
            {1, 1, 1, 1, 1, 1, 1}
    };

    const bool letterSpace[7][7] = {
            {0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0}
    };

    const bool letterError[7][7] = {
            {1, 1, 1, 1, 1, 1, 1},
            {1, 0, 0, 0, 0, 0, 1},
            {1, 0, 1, 1, 1, 0, 1},
            {1, 0, 0, 0, 1, 0, 1},
            {1, 0, 0, 1, 1, 0, 1},
            {1, 0, 0, 0, 0, 0, 1},
            {1, 1, 0, 1, 0, 1, 1}
    };

    const bool letterDash[7][7] = {
            {0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0},
            {0, 1, 1, 1, 1, 1, 0},
            {0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0}
    };

    // All Letters
    const auto letters = {
            letterA, letterB, letterC, letterD, letterE, letterF, letterG,
            letterH, letterI, letterJ, letterK, letterL, letterM, letterN,
            letterO, letterP, letterQ, letterR, letterS, letterT, letterU,
            letterV, letterW, letterX, letterY, letterZ, letterSpace, letterError,
            letterDash
    };

    void ConsoleWindow::insImage(int x, int y, int w, int h, bool *data, int color) {
        for (int i = 0; i < w; i++) {
            for (int j = 0; j < h; j++) {
                if (data[i + j * w]) {
                    setPixel(x + i, y + j, color);
                }
            }
        }
    }

    void ConsoleWindow::placeText(int x, int y, int size, int color, const std::string &text) {
        for (int i = 0; i < text.length(); i++) {
            if (text[i] == ' ') {
                insImage(x + i * size + (3*i), y, size, size, (bool *) letterSpace, color);
            } else {
                char tiUpperCase = text[i];
                if (tiUpperCase >= 'a' && tiUpperCase <= 'z') {
                    tiUpperCase -= 32;
                }

                switch (tiUpperCase) {
                    case 'A':
                        insImage(x + i * size + (3*i), y, size, size, (bool *) letterA, color);
                        break;
                    case 'B':
                        insImage(x + i * size + (3*i), y, size, size, (bool *) letterB, color);
                        break;
                    case 'C':
                        insImage(x + i * size + (3*i), y, size, size, (bool *) letterC, color);
                        break;
                    case 'D':
                        insImage(x + i * size + (3*i), y, size, size, (bool *) letterD, color);
                        break;
                    case 'E':
                        insImage(x + i * size + (3*i), y, size, size, (bool *) letterE, color);
                        break;
                    case 'F':
                        insImage(x + i * size + (3*i), y, size, size, (bool *) letterF, color);
                        break;
                    case 'G':
                        insImage(x + i * size + (3*i), y, size, size, (bool *) letterG, color);
                        break;
                    case 'H':
                        insImage(x + i * size + (3*i), y, size, size, (bool *) letterH, color);
                        break;
                    case 'I':
                        insImage(x + i * size + (3*i), y, size, size, (bool *) letterI, color);
                        break;
                    case 'J':
                        insImage(x + i * size + (3*i), y, size, size, (bool *) letterJ, color);
                        break;
                    case 'K':
                        insImage(x + i * size + (3*i), y, size, size, (bool *) letterK, color);
                        break;
                    case 'L':
                        insImage(x + i * size + (3*i), y, size, size, (bool *) letterL, color);
                        break;
                    case 'M':
                        insImage(x + i * size + (3*i), y, size, size, (bool *) letterM, color);
                        break;
                    case 'N':
                        insImage(x + i * size + (3*i), y, size, size, (bool *) letterN, color);
                        break;
                    case 'O':
                        insImage(x + i * size + (3*i), y, size, size, (bool *) letterO, color);
                        break;
                    case 'P':
                        insImage(x + i * size + (3*i), y, size, size, (bool *) letterP, color);
                        break;
                    case 'Q':
                        insImage(x + i * size + (3*i), y, size, size, (bool *) letterQ, color);
                        break;
                    case 'R':
                        insImage(x + i * size + (3*i), y, size, size, (bool *) letterR, color);
                        break;
                    case 'S':
                        insImage(x + i * size + (3*i), y, size, size, (bool *) letterS, color);
                        break;
                    case 'T':
                        insImage(x + i * size + (3*i), y, size, size, (bool *) letterT, color);
                        break;
                    case 'U':
                        insImage(x + i * size + (3*i), y, size, size, (bool *) letterU, color);
                        break;
                    case 'V':
                        insImage(x + i * size + (3*i), y, size, size, (bool *) letterV, color);
                        break;
                    case 'W':
                        insImage(x + i * size + (3*i), y, size, size, (bool *) letterW, color);
                        break;
                    case 'X':
                        insImage(x + i * size + (3*i), y, size, size, (bool *) letterX, color);
                        break;
                    case 'Y':
                        insImage(x + i * size + (3*i), y, size, size, (bool *) letterY, color);
                        break;
                    case 'Z':
                        insImage(x + i * size + (3*i), y, size, size, (bool *) letterZ, color);
                        break;
                    case '-':
                        insImage(x + i * size + (3*i), y, size, size, (bool *) letterDash, color);
                        break;
                    default:
                        insImage(x + i * size + (3*i), y, size, size, (bool *) letterError, color);
                        break;
                }
            }
        }
    }
}
