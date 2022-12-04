#include <atomic>
#include <X11/Xutil.h>
#include "ConsoleWindow.h"

namespace Ax256::Linux {
    ConsoleWindow::ConsoleWindow() : m_enable(true), m_padding(10), m_gfxHeight(400), m_gfxWidth(500), m_cWidth(800), m_cHeight(400) {
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
        m_graphicsContent = XCreateSimpleWindow(m_display, RootWindow(m_display, screen), 0, 0, m_gfxWidth, m_gfxHeight, 1,
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

        m_thread = new std::thread([this] () {
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

            if (i % 40 == 0) {
                usleep(1);
            }
        }

        generateGrid(0x50FFAB, 0x50FFAB, 5, 5, 30, 30, 10, 10);
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
                    usleep(delayX);
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
}
