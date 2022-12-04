#include "Page.h"
#include <bitset>

namespace Ax256 {
Page::Page(int mb) {
    fprintf(stdout, "[XX3] Allocating %d addresses in RAM.\n", mb);
    for (int i = 0; i < mb; i++) {
        const std::string zeroDat = "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
        std::string addressAsBin = std::bitset<256>(i).to_string();

        m_data[addressAsBin] = zeroDat;
    }
    fprintf(stdout, "[XX3] Done.\n");
}

std::string Page::read(const std::string &address) {
    if (m_data.find(address) != m_data.end()) {
        return m_data[address];
    } else {
        fprintf(stderr, "Page::read, Access violation at address %s\n", address.c_str());
        exit(1);
    }
}

std::string Page::read(uint64_t address) {
    return m_data[std::bitset<256>(address).to_string()];
}
}
