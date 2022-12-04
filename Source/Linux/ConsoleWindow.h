#include <string>
#include <X11/Xlib.h>
#include <thread>

namespace Ax256::Linux {
class ConsoleWindow {
    bool m_enable;
    Display *m_display;
    Window m_window;
    Window m_graphicsContent;

    int m_gfxWidth;
    int m_gfxHeight;

    int m_cWidth;
    int m_cHeight;

    int m_padding;

    int *m_gfxBuffer;

    std::thread *m_thread;

public:
    ConsoleWindow(int w, int h);

    bool setWindowState(bool enable);
    bool write(const std::string &str);

    bool clearGfx();
    bool setPixel(int x, int y, int color);

    bool setGraphicsColor(int color);

    void block();

    int* getGfxBuffer() { return m_gfxBuffer; }

    void generateGrid(int colorX, int colorY, int dashLengthAndSpacingX, int dashLengthAndSpacingY, int divisionX, int divisionY, int delayX, int delayY);

    int width();
    int height();

    void insImage(int x, int y, int w, int h, bool *data, int color);

    void placeText(int x, int y, int size, int color, const std::string& text);
};
}