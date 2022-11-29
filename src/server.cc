#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <functional>
#include <bitset>
#include "regList.hh"
#include "bytedata.h"
#include <thread>
#include <fstream>

// Clock speed hz
int clk = 5;

// Memory height in bytes
const int mbt = 3;

uint64_t pct; // program counter

// Register rows
int reg8 = 10;
int reg16 = 20;

RegisterBox reg8s(0, 0);
RegisterBox reg16s(0, 0);

std::string command;
std::string *args {};
std::string i; // Input

void dumpReg(RegisterBox reg8s) {
    if (reg8s.sections == 0) std::cout << "Empty section list, nothing to display\n";
    else {
        if (args[2] == "all")
            for (int ij = 0; ij < reg8s.sections; ij++) {
                std::string sectorAsStr = std::to_string(ij);
                std::string bit8Address = "0x00000000";
                std::string sectorAsBinaryString = std::bitset<8>(ij).to_string();

                bit8Address.erase(bit8Address.end() - sectorAsBinaryString.length(), bit8Address.end());
                std::cout << bit8Address << sectorAsBinaryString << ": ";

                for (int j = 0; j < reg8s.bits; j++) {
                    std::cout << (int) reg8s.data[ij][j];
                    if (j % 4 == 3) std::cout << " ";
                }
                std::cout << std::endl;
            }
        else {
            try {
                int sectorAsInt = std::stoi(args[2]);
                if (sectorAsInt > reg8s.sections - 1) std::cout << "Section " << sectorAsInt << " does not exist\n";
                else {
                    std::string sectorAsStr = std::to_string(sectorAsInt);
                    std::string bit8Address = "0x00000000";
                    std::string sectorAsBinaryString = std::bitset<8>(sectorAsInt).to_string();

                    bit8Address.erase(bit8Address.end() - sectorAsBinaryString.length(), bit8Address.end());
                    std::cout << bit8Address << sectorAsBinaryString << ": ";

                    for (int j = 0; j < reg8s.bits; j++) {
                        std::cout << (int) reg8s.data[sectorAsInt][j];
                        if (j % 4 == 3) std::cout << " ";
                    }
                    std::cout << std::endl;
                }
            } catch (...) {
                std::cout << "Invalid section number\n";
            }
        }
    }
}

template <typename Out>
void split(const std::string &s, char delim, Out result) {
    std::istringstream iss(s);
    std::string item;
    while (std::getline(iss, item, delim)) {
        *result++ = item;
    }
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}

void work() {
    if (i == "kill") exit(0);
    command = i.substr(0, i.find(' '));
    args = new std::string[i.length()];
    split(i, ' ', args);

    if (command == "dump-r")
        if (args->length() < 3)
            std::cout << "Expected 2 arguments, {bits, (section) | all}, got " << args->length() << std::endl;
        else {
            if (args[1] == "all") {
                std::cout << "Un supported\n";
            } else {
                if (args[1] == "8")
                    dumpReg(reg8s);
                else if (args[1] == "16")
                    dumpReg(reg16s);
                else
                    std::cout << "Invalid register bit width, got " << args[1] << "\n";
            }
        }

    else if (command == "set-r")
        if (args->length() < 4)
            std::cout << "Expected 3 arguments, {bits, section, value}, got " << args->length() << std::endl;
        else {
            if (args[1] == "all") {
                std::cout << "Un supported\n";
            } else {
                if (args[1] == "8") {
                    try {
                        int sectorAsInt = std::stoi(args[2]);
                        if (sectorAsInt > reg8s.sections - 1) std::cout << "Section " << sectorAsInt << " does not exist\n";
                        else {
                            try {
                                int valueAsInt = std::stoi(args[3]);
                                if (valueAsInt > 255) std::cout << "Value " << valueAsInt << " is too large\n";
                                else {
                                    std::string valueAsBin = std::bitset<8>(valueAsInt).to_string();
                                    for (int j = 0; j < reg8s.bits; j++)
                                        reg8s.data[sectorAsInt][j] = valueAsBin[j] - '0';
                                }
                            } catch (...) {
                                std::cout << "Invalid value\n";
                            }
                        }
                    } catch (...) {
                        std::cout << "Invalid section number\n";
                    }
                } else if (args[1] == "16") {
                    try {
                        int sectorAsInt = std::stoi(args[2]);
                        if (sectorAsInt > reg16s.sections - 1) std::cout << "Section " << sectorAsInt << " does not exist\n";
                        else {
                            try {
                                int valueAsInt = std::stoi(args[3]);
                                if (valueAsInt > 65535) std::cout << "Value " << valueAsInt << " is too large\n";
                                else {
                                    std::string valueAsBin = std::bitset<16>(valueAsInt).to_string();
                                    for (int j = 0; j < reg16s.bits; j++)
                                        reg16s.data[sectorAsInt][j] = valueAsBin[j] - '0';
                                }
                            } catch (...) {
                                std::cout << "Invalid value\n";
                            }
                        }
                    } catch (...) {
                        std::cout << "Invalid section number\n";
                    }
                } else
                    std::cout << "Invalid register bit width, got " << args[1] << "\n";
            }
        }

    else if (command == "set-c")
        if (args->length() < 2)
            std::cout << "Expected 1 argument, {value}, got " << args->length() << std::endl;
        else {
            // set the clock speed
            try {
                int valueAsInt = std::stoi(args[1]);
                if (valueAsInt > 1000000) std::cout << "Value " << valueAsInt << " is too large\n";
                else {
                    clk = valueAsInt;
                }
            } catch (...) {
                std::cout << "Invalid value\n";
            }
        }

    else if (command == "jmp")
        if (args->length() < 2)
            std::cout << "Expected 1 argument, {address}, got " << args->length() << std::endl;
        else {
            // jump to address
            try {
                int valueAsInt = std::stoi(args[1]);
                if (valueAsInt > 65535) std::cout << "Value " << valueAsInt << " is too large\n";
                else {
                    pct = valueAsInt;
                }
            } catch (...) {
                std::cout << "Invalid value\n";
            }
        }

    else if (command == "exit" || command == "halt" || command == "quit")
        std::cout << "Did you mean 'kill'?\n";
    else
        std::cout << "Unknown command: " << command << std::endl;
}

std::string to8bithexstring(std::string binaryString256bit) {
    // Take in an input string that is 256 chars long and contains 1s and 0s, convert it into a hexadecimal view that is spaced out every 2 hex digits.
    // Example output: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
    std::string output = "";
    for (int i = 0; i < 256; i += 8) {
        std::string temp = binaryString256bit.substr(i, 8);
        int decimal = std::stoi(temp, nullptr, 2);

        std::stringstream stream;
        stream << std::hex << decimal;
        std::string result(stream.str());
        if (result.length() == 1) result = "0" + result;
        output += result + " ";
    }
    return output;
}

void installProgramBootLoader() {
    // legend: [RAM_WIDTH=256bits, RAM_HEIGHT=2^256]
    // OPCODES (DECIMAL) [OPCODE_BIT_WIDTH=32bits]
    // 0: HLT: Halt the CPU.
    // 1: NOP: Do no operation.
    // 2: JMP (L Value) [ADDRESS 224]: Jump to a specific memory address.

    // 3: JIO (L Value) [ADDRESS 224]: Jump to a specific memory address if there is an ALU overflow.
    // 4: JIZ (L Value) [ADDRESS 224]: Jump to a specific memory address if there is an ALU zero result.
    // 5: JIN (L Value) [ADDRESS 224]: Jump to a specific memory address if there is an ALU negative result.
    // 6: JEQ (L Value, R Value, OPERAND) [FIRST_REGISTER 32, SECOND_REGISTER 32, ADDRESS 160]: Jump to a specific memory address if two registers are equal.
    // 7: JNQ (L Value, R Value, OPERAND) [FIRST_REGISTER 32, SECOND_REGISTER 32, ADDRESS 160]: Jump to a specific memory address if two registers are NOT equal.

    // 8: MOV (L Value, OPERAND) [REGISTER, NUMBER]: Move a value into a CPU register.

    executableByteData = new char*[mbt];

    // nop
    executableByteData[0] = "" "00000000000000000000000000000001" "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";

    // nop
    executableByteData[1] = "" "00000000000000000000000000000001" "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";

    // nop
    executableByteData[2] = "" "00000000000000000000000000000001" "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";

    // nop
    executableByteData[3] = "" "00000000000000000000000000000001" "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";

    // jmp 5
    executableByteData[4] = "" "00000000000000000000000000000010" "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000101";

    // hlt
    executableByteData[5] = "" "00000000000000000000000000000000" "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
}

void loadProgramBootLoader() {
    // Load file relative to executable, EXECPATH/../bootloader.bin
    std::ifstream file;
    file.open("../bootloader", std::ios::binary);

    if (file.is_open()) {
        file.seekg(0, std::ios::end);
        int length = (int) file.tellg();
        file.seekg(0, std::ios::beg);
        char* buffer = new char[length];
        file.read(buffer, length);
        file.close();
        std::cout << "-- Loaded bootloader.bin\n";

        // Each instruction record is 256 bits, so load it accordingly into executableByteData.
        // NOTE: executable byte data is an array of 256 char strings. Each char must either be a 1 or 0, so be sure to convert the bit to a 0 or 1.

        // Loop over all bytes, each byte is 8 bits, so loop over all 8 bits
        std::string asBinaryF;
        for (int ix = 0; ix < length; ix++) {
            auto dx = buffer[ix];
            std::string asBinary = std::bitset<8>(dx).to_string();
            asBinaryF += asBinary;
        }

        // Split binary data into 256 char chunks, and then write them to executableByteData
        int i = 0;
        for (int ix = 0; ix < asBinaryF.length(); ix += 256) {
            std::string asBinary = asBinaryF.substr(ix, 256);
            executableByteData[i] = new char[256];
            for (int j = 0; j < 256; j++)
                executableByteData[i][j] = asBinary[j];
            i++;
        }
    } else {
        std::cout << "-- Failed to load bootloader.bin, booting default program bootloader\n";
        installProgramBootLoader();
    }
}

std::string getIns(std::string record) {
    return record.substr(0, 32);
}

int main(int argc, char *argv[]) {
    reg8s = RegisterBox(8, reg8);
    reg16s = RegisterBox(16, reg16);

    printf("Ax77 Advanced Virtual Machine\n");
    printf("-- Clock speed detect, %d hz\n", clk);
    printf("-- Memory width detect, %d addresses\n", mbt);
    printf("-- 8 bit registers detect, %d sections\n", reg8);
    printf("-- 16 bit registers detect, %d sections\n", reg16);

    printf("-- Booting...\n");
//    installProgramBootLoader();
    loadProgramBootLoader();
    printf("-- Program execution started from bootloader at instruction 0x0 in memory sections\n");
    printf("-- First binary record: %s\n", to8bithexstring(executableByteData[0]).c_str());

    std::thread cpu([&] () {
        std::string rec;
        std::string ins;

        pct = 0; // program counter

        while (true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000 / clk));

            if (pct >= mbt) {
                printf("[!] --> Program counter out of bounds, exiting, counter %lu\n", pct);
                exit(0);
            }

            rec = executableByteData[pct];
            ins = getIns(rec);

            // 0: hlt
            if (ins == "00000000000000000000000000000000") {
                printf("[!] --> CPU ran instruction to halt CPU\n");
                exit(0);
            }

            // 1: nop
            // else if (ins == "00000000000000000000000000000001") {
            //      printf("[!] --> CPU ran instruction to do nothing\n");
            // }

            // 2: jmp
            else if (ins == "00000000000000000000000000000010") {
                std::string addr = rec.substr(32, 256 - 32);
                pct = std::bitset<256 - 32>(addr).to_ulong();
//                printf("[!] --> CPU ran instruction to jump to address %lu\n", pct);
//                printf("-- Current instruction, jumping from %s\n", to8bithexstring(rec).c_str());
//                printf("-- Jumping instruction to %s\n", to8bithexstring(executableByteData[pct]).c_str());
                continue;
            }

            pct++;
        }
    });

    std::function<void()> selfCall = [&selfCall] () {
        std::cout << "Ax77 VM > ";
        std::getline(std::cin, i);
        work();

        selfCall();
    };

    selfCall();
    cpu.join();

    return 0;
}