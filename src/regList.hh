#include <iostream>

class RegisterBox {
public:
    int bits;
    int sections;
    uint8_t **data; // 3d array where the nested arrays are the amount of bits.

    RegisterBox(int bits, int sections) {
        this->bits = bits;
        this->sections = sections;
        data = new uint8_t*[sections];
        for (int i = 0; i < sections; i++)
            data[i] = new uint8_t[bits];
        for (int i = 0; i < sections; i++)
            for (int j = 0; j < bits; j++)
                data[i][j] = 0;
    }
};